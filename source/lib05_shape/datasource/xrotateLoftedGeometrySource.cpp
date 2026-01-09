#include "xrotateLoftedGeometrySource.h"
#include <lib00_utilty/XUtilty.h>

XRotateLoftedgeometrySource::XRotateLoftedgeometrySource()
{
}

XRotateLoftedgeometrySource::~XRotateLoftedgeometrySource()
{
}

void XRotateLoftedgeometrySource::updateVertextCoordArray()
{
	//更新顶点
	//前面的四个坐标点
	std::vector<XQ::Vec2f> points;
	if (isClosed()) {
		points = XQ::math::circleSplit(m_NumVertices, m_angle);
	}
	else
		points = XQ::math::circleSplit(m_NumVertices - 1, m_angle);

	auto singleSurfacenum = m_NumVertices;
	if (isClosed()) {
		singleSurfacenum = m_NumVertices + 1;
	}

	auto rowLen = singleSurfacenum + 1;

	auto bottomPoints = getBottomPoints();
	auto topPoints = getTopPoints();

	m_coord->setNumOfTuple(singleSurfacenum * 2 + 2);		//上下圆心
	int idx = 0;

	//先下底面
	for (int i = 0; i < rowLen; i++) {
		m_coord->setTuple(idx++, bottomPoints[i].x(), bottomPoints[i].y(), -m_height * 0.5);
	}

	//上底面
	for (int i = 0; i < rowLen; i++) {
		m_coord->setTuple(idx++, topPoints[i].x(), topPoints[i].y(), m_height * 0.5);
	}

	m_coord->Modified();
}

void XRotateLoftedgeometrySource::updateIndexArray()
{
	auto singleSurfacePointnum = m_NumVertices;
	if (isClosed()) {
		singleSurfacePointnum = m_NumVertices + 1;
	}

	auto singleSurfaceTrianglenum = singleSurfacePointnum - 1;
	if (isClosed()) {
		m_indexs->setNumOfTuple(singleSurfaceTrianglenum * 2 + singleSurfaceTrianglenum * 2);
	}
	else {
		m_indexs->setNumOfTuple(singleSurfaceTrianglenum * 2 + singleSurfaceTrianglenum * 2 + 4);
	}


	//下底面
	int satrtIdx = 0;
	int idx = 0;
	for (int i = 1; i < singleSurfacePointnum; i++) {
		m_indexs->setTuple(idx++, satrtIdx, satrtIdx + i, satrtIdx + i + 1);
	}

	//上底面
	satrtIdx = singleSurfacePointnum + 1;
	for (int i = 1; i < singleSurfacePointnum; i++) {
		m_indexs->setTuple(idx++, satrtIdx, satrtIdx + i, satrtIdx + i + 1);
	}

	//1-singleSurfacePointnum
	for (int i = 0; (i + 1) < singleSurfacePointnum; i++) {
		auto firstRowStart = i + 1;
		auto secondRowStart = singleSurfacePointnum + 2 + i;
		m_indexs->setTuple(idx++, firstRowStart, secondRowStart, secondRowStart + 1);
		m_indexs->setTuple(idx++, firstRowStart, secondRowStart + 1, firstRowStart + 1);
	}

	if (!isClosed()) {
		auto rowLen = singleSurfacePointnum + 1;
		auto centerBottomIdx = 0;
		auto centerTopIdx = rowLen;
		auto firstRowStart = 1;
		auto firstRowEnd = rowLen - 1;
		auto secondRowStart = rowLen + 1;
		auto secondRowEnd = 2 * rowLen - 1;

		m_indexs->setTuple(idx++, centerBottomIdx, firstRowStart, secondRowStart);
		m_indexs->setTuple(idx++, centerBottomIdx, secondRowStart, centerTopIdx);

		m_indexs->setTuple(idx++, centerBottomIdx, centerTopIdx, secondRowEnd);
		m_indexs->setTuple(idx++, centerBottomIdx, secondRowEnd, firstRowEnd);
	}

	m_indexs->Modified();
}

void XRotateLoftedgeometrySource::updateFaceColorArray()
{
	auto singleSurfacePointnum = m_NumVertices;
	if (isClosed()) {
		singleSurfacePointnum = m_NumVertices + 1;
	}
	auto singleSurfaceTrianglenum = singleSurfacePointnum - 1;

	if (isClosed()) {
		m_FaceColor->setNumOfTuple(singleSurfaceTrianglenum * 2 + singleSurfaceTrianglenum * 2);
	}
	else {
		m_FaceColor->setNumOfTuple(singleSurfaceTrianglenum * 2 + singleSurfaceTrianglenum * 2 + 4);
	}

	for (int i = 0; i < singleSurfaceTrianglenum; i++)
	{
		m_FaceColor->setTuple(i, 1, 0, 0);
	}

	for (int i = 0; i < singleSurfaceTrianglenum; i++)
	{
		m_FaceColor->setTuple(i + singleSurfaceTrianglenum, 0, 1, 0);
	}


	for (int i = 0; i < singleSurfaceTrianglenum * 2; i++)
	{
		m_FaceColor->setTuple(i + 2 * singleSurfaceTrianglenum, 0, 0, 1);
	}

	if (!isClosed()) {
		m_FaceColor->setTuple(4 * singleSurfaceTrianglenum + 0, 1, 1, 1);
		m_FaceColor->setTuple(4 * singleSurfaceTrianglenum + 1, 1, 1, 1);
		m_FaceColor->setTuple(4 * singleSurfaceTrianglenum + 2, 0, 1, 1);
		m_FaceColor->setTuple(4 * singleSurfaceTrianglenum + 3, 0, 1, 1);
	}

	m_FaceColor->Modified();
}

void XRotateLoftedgeometrySource::updateNormalArray()
{
	m_normal->Modified();
}

void XRotateLoftedgeometrySource::updateVertexColorArray()
{
	m_VertexColor->Modified();
}

bool XRotateLoftedgeometrySource::isClosed() const
{
	return m_angle == 360;
}


void XRotateLoftedgeometrySource::setNumVertices(unsigned int numVertices) {
	m_NumVertices = numVertices;
}

void XRotateLoftedgeometrySource::setAngle(double angle) {
	m_angle = angle;
}

//std::vector<XQ::Vec3f> XRotateLoftedgeometrySource::getBottomPoints() {
//	std::vector<XQ::Vec3f> result;
//	std::vector<XQ::Vec2f> tmp;
//	if (isClosed()) {
//		tmp = XQ::math::circleSplit(m_NumVertices, m_angle);
//	}
//	else {
//		tmp = XQ::math::circleSplit(m_NumVertices - 1, m_angle);
//	}
//
//	result.resize(tmp.size() + 1);
//	result[0] = XQ::Vec3f(0, 0, -m_height * 0.5);
//	for (int i = 0; i < tmp.size(); i++) {
//		result[i + 1] = XQ::Vec3f(tmp[i].x, tmp[i].y, -m_height * 0.5);
//	}
//
//	return result;
//}
//
//std::vector<XQ::Vec3f> XRotateLoftedgeometrySource::getTopPoints() {
//	std::vector<XQ::Vec3f> result;
//	std::vector<XQ::Vec2f> tmp;
//	if (isClosed()) {
//		tmp = XQ::math::circleSplit(m_NumVertices, m_angle);
//	}
//	else {
//		tmp = XQ::math::circleSplit(m_NumVertices - 1, m_angle);
//	}
//
//	result.resize(tmp.size() + 1);
//	result[0] = XQ::Vec3f(0, 0, m_height * 0.5);
//	for (int i = 0; i < tmp.size(); i++) {
//		result[i + 1] = XQ::Vec3f(tmp[i].x, tmp[i].y, m_height * 0.5);
//	}
//
//	return result;
//}