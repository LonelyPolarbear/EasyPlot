#include "xfrustumSource.h"
#include <dataBase/XVector.h>

//用于绘制视锥体
//
xfrustumSource::xfrustumSource()
{
}

xfrustumSource::~xfrustumSource()
{
}


std::vector<XQ::Vec3f> xfrustumSource::getBottomPoints() {
	if (!hasPoint())
		return {};
	XQ::Vec3f average;
	for (auto s : mFarPlanePoint) {
		average+=s;
	}
	average /= mFarPlanePoint.size();

	std::vector<XQ::Vec3f> result;
	result.resize(m_NumVertices +2);
	result[0] = average;
	for (int i = 0; i < m_NumVertices; i++) {
		result[i + 1] = mFarPlanePoint[i];
	}
	result[m_NumVertices+1] = mFarPlanePoint[0];

	return result;
}

std::vector<XQ::Vec3f> xfrustumSource::getTopPoints() {
	if (!hasPoint())
		return {};
	XQ::Vec3f average;
	for (auto s : mNearPlanePoint) {
		average += s;
	}
	average /= mNearPlanePoint.size();
	std::vector<XQ::Vec3f> result;
	result.resize(m_NumVertices + 2);
	result[0] = average;
	for (int i = 0; i < m_NumVertices ; i++) {
		result[i + 1] = mNearPlanePoint[i];
	}
	result[m_NumVertices + 1] = mNearPlanePoint[0];
	return result;
}

void xfrustumSource::setFarPlanePoints(std::vector<XQ::Vec3f> p)
{
	m_NumVertices = p.size();
	mFarPlanePoint = p;
	Modified();
}

void xfrustumSource::setNearPlanePoints(std::vector<XQ::Vec3f> p)
{
	mNearPlanePoint =p;
	Modified();
}

void xfrustumSource::updateVertextCoordArray()
{
	//更新顶点
	//前面的四个坐标点
	//下底面面中心点 bot[0] bot[1] bot[2] bot[3] bot[0]
	//上底面面中心点 top[0] top[1] top[2] top[3] top[0]
	if (!hasPoint())
		return;
	auto bottomPoints = getBottomPoints();
	auto topPoints = getTopPoints();

	auto singleSurfacenum = m_NumVertices;
	if (isClosed()) {
		singleSurfacenum = m_NumVertices + 1;
	}

	auto rowLen = singleSurfacenum + 1;

	m_VertexCoord->setNumOfTuple(singleSurfacenum * 2 + 2);		//上下圆心
	int idx = 0;

	//上底面
	for (int i = 0; i < rowLen; i++) {
		m_VertexCoord->setTuple(idx++, topPoints[i].x(), topPoints[i].y(), topPoints[i].z());
	}

	//先下底面
	for (int i = 0; i < rowLen; i++) {
		m_VertexCoord->setTuple(idx++, bottomPoints[i].x(), bottomPoints[i].y(), bottomPoints[i].z());
	}

	m_VertexCoord->Modified();
}

#if 0
void xfrustumSource::updateLineIndexArray()
{
	//底面绘制 4条线 8个点
	//顶面绘制4条线 8个点
	//侧面绘制4条线 8个点
	//下底面面中心点 bot[0] bot[1] bot[2] bot[3] bot[0]
	//上底面面中心点 top[0] top[1] top[2] top[3] top[0]
	m_LineIndexs->setNumOfTuple(12);

	m_LineIndexs->setTuple(0,1,2);
	m_LineIndexs->setTuple(1,2,3);
	m_LineIndexs->setTuple(2, 3, 4);
	m_LineIndexs->setTuple(3, 4, 1);

	m_LineIndexs->setTuple(4, 1+6, 2+6);
	m_LineIndexs->setTuple(5, 2+6, 3+6);
	m_LineIndexs->setTuple(6, 3+6, 4+6);
	m_LineIndexs->setTuple(7, 4+6, 1+6);

	m_LineIndexs->setTuple(8, 1, 1 + 6);
	m_LineIndexs->setTuple(9, 2, 2 + 6);
	m_LineIndexs->setTuple(10, 3, 3 + 6);
	m_LineIndexs->setTuple(11, 4, 4 + 6);


	m_LineIndexs->Modified();
}
#endif
