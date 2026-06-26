#include "xpolygonLineSource.h"
#include <iostream>
#include <fstream>
#include <QString>
#include <QStringList>
#include <QRegularExpression>
#include <QFile>
#include <regex>
#include <QSet>

class XPolygonLineSource::Internal {
public:
	sptr<XFloatArray> polygonLineCoords;
	Internal() {
		polygonLineCoords = makeShareDbObject<XFloatArray>();
		polygonLineCoords->setComponent(3);
	}
};

XPolygonLineSource::XPolygonLineSource():mData(new Internal)
{

}

XPolygonLineSource::~XPolygonLineSource()
{
}

void XPolygonLineSource::updateVertextCoordArray()
{
	
	m_VertexCoord->Modified();
	return;
	if (mData->polygonLineCoords->IsModifiedAfter(m_VertexCoord)) {
		//更新时间
		 {
			int oldNum = m_VertexCoord->getNumOfTuple();

			if (m_VertexCoord) {
				if (mData->polygonLineCoords->getNumOfTuple() != oldNum + 2) {
					mData->polygonLineCoords->setNumOfTuple(oldNum+2);
				}
			}
			auto polygonLineCoords = mData->polygonLineCoords;
			auto addAdjacency = m_VertexCoord;
			//原始数据只有四个点，添加三个额外的点

			// 1 2 3 4
			// 1 1 2 3 4 4
			addAdjacency->setNumOfTuple(oldNum + 2);

			XQ::Vec3f first_point = XQ::Vec2f(polygonLineCoords->data(0)[0], polygonLineCoords->data(0)[1], polygonLineCoords->data(0)[2]);
			XQ::Vec3f last_point = XQ::Vec2f(polygonLineCoords->data(oldNum-1)[0], polygonLineCoords->data(oldNum - 1)[1], polygonLineCoords->data(oldNum - 1)[2]);
			


			//设置第一个点
			addAdjacency->data(0)[0] = first_point.x();
			addAdjacency->data(0)[1] = first_point.y();
			addAdjacency->data(0)[2] = 0;

			//拷贝原始数据

			memcpy(addAdjacency->data(1), polygonLineCoords->data(0), oldNum * 3 * sizeof(float));

			//最后一个点
			addAdjacency->data(oldNum + 1)[0] = last_point.x();
			addAdjacency->data(oldNum + 1)[1] = last_point.y();
			addAdjacency->data(oldNum + 1)[2] = 0;

			//m_vbo_coord->allocate(addAdjacency->data(0), addAdjacency->size());

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
			//m_ssbo_len->bind();
			//m_ssbo_len->allocate(lens->data(0), lens->size());
			//m_ssbo_len->release();
		}
		m_VertexCoord->Modified();
	}
}

void XPolygonLineSource::updateFaceIndexArray()
{
	//m_FaceIndexs->Modified();
}

void XPolygonLineSource::updateFaceColorArray()
{
	//m_FaceColor->Modified();
}

void XPolygonLineSource::updateVertextNormalArray()
{
	//m_VertexNormal->Modified();
}

void XPolygonLineSource::updateVertexColorArray()
{
	//m_VertexColor->Modified();
}

void XPolygonLineSource::updateLineIndexArray()
{
	
}

void XPolygonLineSource::updateLineColorArray()
{
}

void XPolygonLineSource::updateVertexIndexArray()
{
}

void XPolygonLineSource::updateInstancedArray()
{
	XShapeSource::updateInstancedArray();
}

sptr<XFloatArray> XPolygonLineSource::getPolygonLineCoords() const
{
	return m_VertexCoord;
}
