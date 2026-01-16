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
	if (!hasPoint())
		return;
	auto bottomPoints = getBottomPoints();
	auto topPoints = getTopPoints();

	auto singleSurfacenum = m_NumVertices;
	if (isClosed()) {
		singleSurfacenum = m_NumVertices + 1;
	}

	auto rowLen = singleSurfacenum + 1;

	m_coord->setNumOfTuple(singleSurfacenum * 2 + 2);		//上下圆心
	int idx = 0;

	//先下底面
	for (int i = 0; i < rowLen; i++) {
		m_coord->setTuple(idx++, bottomPoints[i].x(), bottomPoints[i].y(), bottomPoints[i].z());
	}

	//上底面
	for (int i = 0; i < rowLen; i++) {
		m_coord->setTuple(idx++, topPoints[i].x(), topPoints[i].y(), topPoints[i].z());
	}

	m_coord->Modified();
}
