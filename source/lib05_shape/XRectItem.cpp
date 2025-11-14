#include "XRectItem.h"
#include <lib04_opengl/XOpenGLBuffer.h>

XRectItem::XRectItem(std::shared_ptr<XGraphicsItem> parent):XGraphicsItem(parent)
{
	this->setDrawType(PrimitveType::line_strip_adjacency);
	auto coord = makeShareDbObject<XFloatArray>();

	coord->setComponent(3);
	coord->setNumOfTuple(4);

	coord->setTuple(0, -1, -1, 0);
	coord->setTuple(1, 1, -1, 0);
	coord->setTuple(2, 1, 1, 0);
	coord->setTuple(3, -1, 1, 0);

	this->setCoordArray(coord);

	auto index = makeShareDbObject<XUIntArray>();
	index->setComponent(3);
	index->setNumOfTuple(2);
	index->setTuple(0, 1, 2, 3);
	index->setTuple(1, 1, 3, 4);

	this->setIndexArray(index);
}

XRectItem::~XRectItem()
{
}

void XRectItem::setRectType(XRectItem::RectType rectType)
{
	m_rectType = rectType;
}

XRectItem::RectType XRectItem::getRectType() const
{
	return m_rectType;
}

void XRectItem::setRect(myUtilty::Vec2f pos1, myUtilty::Vec2f pos2)
{
	if (m_rectType == XRectItem::RectType::diagonal_rect) {
		auto points = myUtilty::math::getRectCorner(pos1, pos2);
		m_coordArray->setTuple(0, points[0].x, points[0].y, 0);
		m_coordArray->setTuple(1, points[1].x, points[1].y, 0);
		m_coordArray->setTuple(2, points[2].x, points[2].y, 0);
		m_coordArray->setTuple(3, points[3].x, points[3].y, 0);
		m_coordArray->Modified();
	}
	else if (m_rectType == XRectItem::RectType::center_rect) {
		myUtilty::Vec2f corner1  = pos2;
		myUtilty::Vec2f corner2  = 2.f*pos1 - pos2;

		auto points = myUtilty::math::getRectCorner(corner1, corner2);
		m_coordArray->setTuple(0, points[0].x, points[0].y, 0);
		m_coordArray->setTuple(1, points[1].x, points[1].y, 0);
		m_coordArray->setTuple(2, points[2].x, points[2].y, 0);
		m_coordArray->setTuple(3, points[3].x, points[3].y, 0);
		m_coordArray->Modified();
	
	}
}

void XRectItem::updateVboCoord()
{
	//顶点数据已经更新
	auto m_coord = m_coordArray;
	if (m_coord && m_coord->GetTimeStamp() > m_UpdateTime) {

		m_vbo_coord->bind();

		if (m_drawType == PrimitveType::line_strip_adjacency) {
			int oldNum = m_coord->getNumOfTuple();
			auto addAdjacency = makeShareDbObject<XFloatArray>();
			addAdjacency->setComponent(3);
			//原始数据只有四个点，添加三个额外的点

			// 1 2 3 4
			// 4 1 2 3 4 1 2
			addAdjacency->setNumOfTuple(oldNum + 3);
			memcpy(addAdjacency->data(1), m_coord->data(0), m_coord->size() * sizeof(float));

			//设置第一个点
			addAdjacency->data(0)[0] = m_coord->data(oldNum - 1)[0];
			addAdjacency->data(0)[1] = m_coord->data(oldNum - 1)[1];
			addAdjacency->data(0)[2] = m_coord->data(oldNum - 1)[2];

			//倒数第二个点
			addAdjacency->data(oldNum + 1)[0] = m_coord->data(0)[0];
			addAdjacency->data(oldNum + 1)[1] = m_coord->data(0)[1];
			addAdjacency->data(oldNum + 1)[2] = m_coord->data(0)[2];

			//最后一个点
			addAdjacency->data(oldNum + 2)[0] = m_coord->data(1)[0];
			addAdjacency->data(oldNum + 2)[1] = m_coord->data(1)[1];
			addAdjacency->data(oldNum + 2)[2] = m_coord->data(1)[2];

			m_vbo_coord->allocate(addAdjacency->data(0), addAdjacency->size());
			m_indexArray->setTuple(0, 1, 2, 3);
			m_indexArray->setTuple(1, 1, 3, 4);
			m_indexArray->Modified();
		}
		else {
			m_vbo_coord->allocate(m_coord->data(0), m_coord->size());
			m_indexArray->setTuple(0, 0, 1, 2);
			m_indexArray->setTuple(1, 0, 2, 3);
			m_indexArray->Modified();
		}

		m_vbo_coord->release();
	}
}

uint32_t XRectItem::computeNumofVertices()
{
	if(m_drawType == PrimitveType::line_strip_adjacency)
		return m_coordArray->getNumOfTuple()+3;
	else
		return m_coordArray->getNumOfTuple();
}
