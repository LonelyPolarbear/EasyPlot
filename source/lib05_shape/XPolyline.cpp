#include "XPolyline.h"
#include <lib04_opengl/XOpenGLBuffer.h>

XPolyline::XPolyline():XGraphicsItem()
{
	this->setDrawType(PrimitveType::line_strip_adjacency);
	auto coord = makeShareDbObject<XFloatArray>();

	coord->setComponent(3);
	coord->setNumOfTuple(2);

	coord->setTuple(0, 0, 0, 0);
	coord->setTuple(1, 1, 0, 0);

	this->setCoordArray(coord);

	m_hasSSBOLen = true;
}

XPolyline::~XPolyline()
{
}

void XPolyline::updateVboCoord()
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
			addAdjacency->setNumOfTuple(oldNum + 2);

			myUtilty::Vec2f p1 = myUtilty::Vec2f(m_coord->data(0)[0], m_coord->data(0)[1]);
			myUtilty::Vec2f p2 = myUtilty::Vec2f(m_coord->data(1)[oldNum - 1], m_coord->data(1)[1]);


			//设置第一个点
			addAdjacency->data(0)[0] = p1.x;
			addAdjacency->data(0)[1] = p1.y;
			addAdjacency->data(0)[2] = 0;

			//拷贝原始数据

			memcpy(addAdjacency->data(1), m_coord->data(0), oldNum * 3 * sizeof(float));

			//最后一个点
			addAdjacency->data(oldNum + 1)[0] = p2.x;
			addAdjacency->data(oldNum + 1)[1] = p2.y;
			addAdjacency->data(oldNum + 1)[2] = 0;

			m_vbo_coord->allocate(addAdjacency->data(0), addAdjacency->size());

			//oldNum个有效点 存储oldnum段数据
			auto lens = makeShareDbObject<XFloatArray>();
			lens->setNumOfTuple(oldNum);
			lens->setTuple(0, 0);
			for (int i = 1; i < oldNum; i++) {
				auto lastPoint = addAdjacency->data(i + 1);		//跳过第一个点
				auto curPoint = addAdjacency->data(i + 2);		//跳过第一个点

				Eigen::Vector2f v1(lastPoint[0], lastPoint[1]);
				Eigen::Vector2f v2(curPoint[0], curPoint[1]);
				Eigen::Vector2f v3 = v2 - v1;
				float len = v3.norm();
				lens->setTuple(i, len + *lens->data(i - 1));
			}
			m_ssbo_len->bind();
			m_ssbo_len->allocate(lens->data(0), lens->size());
			m_ssbo_len->release();
		}
		else {
			m_vbo_coord->allocate(m_coord->data(0), m_coord->size());
			int oldNum = m_coord->getNumOfTuple();
			auto lens = makeShareDbObject<XFloatArray>();
			lens->setNumOfTuple(oldNum);
			lens->setTuple(0, 0);
			for (int i = 1; i < oldNum; i++) {
				auto lastPoint = m_coord->data(i);		//跳过第一个点
				auto curPoint = m_coord->data(i + 1);		//跳过第一个点

				Eigen::Vector2f v1(lastPoint[0], lastPoint[1]);
				Eigen::Vector2f v2(curPoint[0], curPoint[1]);
				Eigen::Vector2f v3 = v2 - v1;
				float len = v3.norm();
				lens->setTuple(i, len + *lens->data(i - 1));
			}
			m_ssbo_len->bind();
			m_ssbo_len->allocate(lens->data(0), lens->size());
			m_ssbo_len->release();
		}

		m_vbo_coord->release();
	}
}

uint32_t XPolyline::computeNumofVertices()
{
	return m_coordArray->getNumOfTuple()+2;
}

void XPolyline::pickBorderDraw(std::shared_ptr<xshader> shader, const Eigen::Matrix4f& m)
{
	return XGraphicsItem::pickBorderDraw(shader,m);
}

void XPolyline::pickFillDraw(std::shared_ptr<xshader> shader, const Eigen::Matrix4f& m)
{
	//return XGraphicsItem::pickFillDraw(shader, m);
}
