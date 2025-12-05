#include "XLineItem.h"
#include <lib04_opengl/XOpenGLBuffer.h>

XLineItem::XLineItem(std::shared_ptr<XGraphicsItem> parent):XGraphicsItem(parent)
{
	this->setDrawType(PrimitveType::line_strip_adjacency);
	auto coord = makeShareDbObject<XFloatArray>();

	coord->setComponent(3);
	coord->setNumOfTuple(2);

	coord->setTuple(0, 0, 0, 0);
	coord->setTuple(1, 1, 0, 0);

	this->setCoordArray(coord);
}

XLineItem::~XLineItem()
{
}

void XLineItem::updateVboCoord()
{
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
			addAdjacency->setNumOfTuple(oldNum + 2);

			myUtilty::Vec2f p1 = myUtilty::Vec2f(m_coord->data(0)[0], m_coord->data(0)[1]);
			myUtilty::Vec2f p2 = myUtilty::Vec2f(m_coord->data(1)[0], m_coord->data(1)[1]);

			auto pre = 2.0f * p1 - p2;
			auto next = 2.0f * p2 - p1;

			//设置第一个点
			addAdjacency->data(0)[0] = p1.x();
			addAdjacency->data(0)[1] = p1.y();
			addAdjacency->data(0)[2] = 0;

			addAdjacency->data(1)[0] = p1.x();
			addAdjacency->data(1)[1] = p1.y();
			addAdjacency->data(1)[2] = 0;

			addAdjacency->data(2)[0] = p2.x();
			addAdjacency->data(2)[1] = p2.y();
			addAdjacency->data(2)[2] = 0;


			//最后一个点
			addAdjacency->data(3)[0] = p2.x();
			addAdjacency->data(3)[1] = p2.y();
			addAdjacency->data(3)[2] = 0;

			m_vbo_coord->allocate(addAdjacency->data(0), addAdjacency->size());
		}
		else {
			m_vbo_coord->allocate(m_coord->data(0), m_coord->size());
		}

		m_vbo_coord->release();
	}
}

void XLineItem::setLine(const myUtilty::Vec2f& start, const myUtilty::Vec2f& end)
{
	m_coordArray->setTuple(0, start.x(), start.y(), 0);
	m_coordArray->setTuple(1, end.x(), end.y(), 0);
	m_coordArray->Modified();
}

myUtilty::Vec2f XLineItem::getStart() const
{
 	return myUtilty::Vec2f(m_coordArray->data(0)[0], m_coordArray->data(0)[1]);
}

myUtilty::Vec2f XLineItem::getEnd() const
{
	return myUtilty::Vec2f(m_coordArray->data(1)[0], m_coordArray->data(1)[1]);
}

double XLineItem::getLength() const
{
	auto tmp =getEnd() - getStart();
	return sqrtf(tmp.x()*tmp.x() + tmp.y() * tmp.y());
}

uint32_t XLineItem::computeNumofVertices()
{
	return 4;
}
