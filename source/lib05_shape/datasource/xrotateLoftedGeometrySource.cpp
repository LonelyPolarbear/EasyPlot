#include "xrotateLoftedGeometrySource.h"
#include <dataBase/XVector.h>

XRotateLoftedgeometrySource::XRotateLoftedgeometrySource()
{
}

XRotateLoftedgeometrySource::~XRotateLoftedgeometrySource()
{
}

void XRotateLoftedgeometrySource::Init()
{
	initSource = true;
}

void XRotateLoftedgeometrySource::updateVertextCoordArray()
{
	//更新顶点
	//前面的四个坐标点
	auto bottomPoints = getBottomPoints();
	auto topPoints = getTopPoints();

	if (bottomPoints.size() == 0)
		return;

	auto singleSurfacenum = m_NumVertices;
	if (isClosed()) {
		singleSurfacenum = m_NumVertices + 1;
	}

	auto rowLen = singleSurfacenum + 1;

	m_VertexCoord->setNumOfTuple(singleSurfacenum * 2 + 2);		//上下圆心
	int idx = 0;

	//上底面
	for (int i = 0; i < rowLen; i++) {
		m_VertexCoord->setTuple(idx++, topPoints[i].x(), topPoints[i].y(), m_height * 0.5);
	}

	//先下底面
	for (int i = 0; i < rowLen; i++) {
		m_VertexCoord->setTuple(idx++, bottomPoints[i].x(), bottomPoints[i].y(), -m_height * 0.5);
	}

	m_VertexCoord->Modified();
}

void XRotateLoftedgeometrySource::updateFaceIndexArray()
{
	auto bottomPoints = getBottomPoints();
	auto topPoints = getTopPoints();

	if (bottomPoints.size() == 0)
		return;

	auto singleSurfacePointnum = m_NumVertices;
	if (isClosed()) {
		singleSurfacePointnum = m_NumVertices + 1;
	}

	auto singleSurfaceTrianglenum = singleSurfacePointnum - 1;
	if (isClosed()) {
		m_FaceIndexs->setNumOfTuple(singleSurfaceTrianglenum * 2 + singleSurfaceTrianglenum * 2);
	}
	else {
		m_FaceIndexs->setNumOfTuple(singleSurfaceTrianglenum * 2 + singleSurfaceTrianglenum * 2 + 4);
	}

	//先上顶面
	int satrtIdx = 0;
	int idx = 0;
	for (int i = 1; i < singleSurfacePointnum; i++) {
		m_FaceIndexs->setTuple(idx++, satrtIdx, satrtIdx + i, satrtIdx + i + 1);
	}

	//下底面 需要反转顺序
	satrtIdx = singleSurfacePointnum + 1;
	for (int i = 1; i < singleSurfacePointnum; i++) {
		m_FaceIndexs->setTuple(idx++, satrtIdx, satrtIdx + i + 1, satrtIdx + i);
	}

	//1-singleSurfacePointnum
	for (int i = 0; (i + 1) < singleSurfacePointnum; i++) {
		auto firstRowStart = i + 1;
		auto secondRowStart = singleSurfacePointnum + 2 + i;
		m_FaceIndexs->setTuple(idx++, firstRowStart, secondRowStart, secondRowStart + 1);
		m_FaceIndexs->setTuple(idx++, firstRowStart, secondRowStart + 1, firstRowStart + 1);
	}

	if (!isClosed()) {
		auto rowLen = singleSurfacePointnum + 1;
		auto centerBottomIdx = 0;
		auto centerTopIdx = rowLen;
		auto firstRowStart = 1;
		auto firstRowEnd = rowLen - 1;
		auto secondRowStart = rowLen + 1;
		auto secondRowEnd = 2 * rowLen - 1;

		m_FaceIndexs->setTuple(idx++, centerBottomIdx, firstRowStart, secondRowStart);
		m_FaceIndexs->setTuple(idx++, centerBottomIdx, secondRowStart, centerTopIdx);

		m_FaceIndexs->setTuple(idx++, centerBottomIdx, centerTopIdx, secondRowEnd);
		m_FaceIndexs->setTuple(idx++, centerBottomIdx, secondRowEnd, firstRowEnd);
	}

	m_FaceIndexs->Modified();
}

void XRotateLoftedgeometrySource::updateFaceColorArray()
{
	auto bottomPoints = getBottomPoints();
	if (bottomPoints.size() == 0)
		return;

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
		m_FaceColor->setTuple(i, 1, 0, 0, 1);
	}

	for (int i = 0; i < singleSurfaceTrianglenum; i++)
	{
		m_FaceColor->setTuple(i + singleSurfaceTrianglenum, 0, 1, 0, 1);
	}


	for (int i = 0; i < singleSurfaceTrianglenum * 2; i++)
	{
		m_FaceColor->setTuple(i + 2 * singleSurfaceTrianglenum, 0, 0, 1, 1);
	}

	if (!isClosed()) {
		m_FaceColor->setTuple(4 * singleSurfaceTrianglenum + 0, 1, 1, 1, 1);
		m_FaceColor->setTuple(4 * singleSurfaceTrianglenum + 1, 1, 1, 1, 1);
		m_FaceColor->setTuple(4 * singleSurfaceTrianglenum + 2, 0, 1, 1, 1);
		m_FaceColor->setTuple(4 * singleSurfaceTrianglenum + 3, 0, 1, 1, 1);
	}

	m_FaceColor->Modified();
}

void XRotateLoftedgeometrySource::updateVertextNormalArray()
{
	m_VertexNormal->Modified();
}

void XRotateLoftedgeometrySource::updateVertexColorArray()
{
	m_VertexColor->Modified();
}

void XRotateLoftedgeometrySource::updateLineIndexArray()
{
	auto bottomPoints = getBottomPoints();
	if (bottomPoints.size() == 0)
		return;

	//如果是闭合，则内部的中心点不需要连线，一共m_NumVertices个点
	//顶面中心 0 1 2 3 4 ... n 0   //m_NumVertices+2
	//底面中心 0 1 2 3 4 ... n 0   //m_NumVertices+2
	if (isClosed()) {
		auto lineNum = m_NumVertices * 2/*+m_NumVertices*/;//上下面 闭合的回转体不需要棱
		m_LineIndexs->setNumOfTuple(lineNum);

		for (int i = 0; i < m_NumVertices; i++) {
			m_LineIndexs->setTuple(i, i + 1, i + 2);
		}

		int createdLineNum = m_NumVertices;
		int startVertexIdx = m_NumVertices + 2;

		for (int i = 0; i < m_NumVertices; i++) {
			m_LineIndexs->setTuple(i + createdLineNum, i + 1+ startVertexIdx, i + 2+ startVertexIdx);
		}

		m_LineIndexs->Modified();
	}
	else {
		//顶面中心 0 1 2 3 4 ... n   //m_NumVertices+1
		//底面中心 0 1 2 3 4 ... n   //m_NumVertices+1
		//不闭合，此时直线需要连接中心点
		//每个面一共m_NumVertices+1条线 侧边棱 不闭合的回转体侧边棱三条
		auto lineNum = (m_NumVertices+1) * 2 + /*m_NumVertices+1*/3;
		m_LineIndexs->setNumOfTuple(lineNum);

		//顶面的边
		for (int i = 0; i < m_NumVertices+1; i++) {
			m_LineIndexs->setTuple(i, i, (i + 1)%(m_NumVertices + 1));
		}

		int createdLineNum = m_NumVertices+1;
		int startVertexIdx = m_NumVertices + 1;

		for (int i = 0; i < m_NumVertices + 1; i++) {
			m_LineIndexs->setTuple(i+createdLineNum, i+ m_NumVertices + 1, (i + 1) % (m_NumVertices + 1) + startVertexIdx);
		}

		//侧棱 中心线 起点-起点 终点-终点
		createdLineNum += m_NumVertices+1;
		
		m_LineIndexs->setTuple(createdLineNum++, 0, 0 + m_NumVertices + 1);
		m_LineIndexs->setTuple(createdLineNum++, 1, 1 + m_NumVertices + 1);
		m_LineIndexs->setTuple(createdLineNum++, m_NumVertices, m_NumVertices + m_NumVertices + 1);

		m_LineIndexs->Modified();
	}
}

void XRotateLoftedgeometrySource::updateLineColorArray()
{
	m_LineColor->Modified();
}

void XRotateLoftedgeometrySource::updateVertexIndexArray()
{
	m_VertexIndexs->Modified();
}

bool XRotateLoftedgeometrySource::isClosed() const
{
	return m_angle == 360.0;
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