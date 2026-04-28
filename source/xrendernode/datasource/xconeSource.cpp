#include "xconeSource.h"
#include <xalgo/XAlgo.h>

XConeSource::XConeSource()
{
	m_NumVertices = 360;
	m_angle = 360;
}

XConeSource::~XConeSource()
{
}


void XConeSource::updateVertexIndexArray()
{
	if (isClosed()) {
		//to,bo,p0,p1,p2,...pn,p0
		m_VertexIndexs->setNumOfTuple(m_NumVertices+1);
		m_VertexIndexs->setTuple(0, 0);
		for (int i = 0; i < m_NumVertices; i++) {
			m_VertexIndexs->setTuple(1+i, 2+i);
		}
	}
	else {
		//to,bo,p0,p1,p2,...pn
		m_VertexIndexs->setNumOfTuple(m_NumVertices + 2);
		for (int i = 0; i < m_NumVertices+2; i++) {
			m_VertexIndexs->setTuple(i, i);
		}
	}
	m_VertexIndexs->Modified();
}

void XConeSource::setNumVertices(unsigned int numVertices)
{
	XRotateLoftedgeometrySource::setNumVertices(numVertices);
}

void XConeSource::setAngle(double angle)
{
	XRotateLoftedgeometrySource::setAngle(angle);
}

std::vector<XQ::Vec3f> XConeSource::getBottomPoints() {
	std::vector<XQ::Vec3f> result;
	std::vector<XQ::Vec2f> tmp;
	//如果闭合一共m_NumVertices+2个点
	//如果不闭合一共m_NumVertices+1个点
	if (isClosed()) {
		tmp = XQ::math::circleSplit(m_NumVertices, m_angle);
	}
	else {
		tmp = XQ::math::circleSplit(m_NumVertices - 1, m_angle);
	}

	result.resize(tmp.size() + 1);
	result[0] = XQ::Vec3f(0, 0, -m_height * 0.5);
	for (int i = 0; i < tmp.size(); i++) {
		result[i + 1] = XQ::Vec3f(tmp[i].x(), tmp[i].y(), -m_height * 0.5);
	}

	return result;
}

std::vector<XQ::Vec3f> XConeSource::getTopPoints() {
	return { XQ::Vec3f(0, 0, m_height * 0.5) };
}

void XConeSource::updateVertextCoordArray()
{
	auto bottomPoints = getBottomPoints();
	auto topPoints = getTopPoints();

	//顶点只有一个点 
	auto singleSurfacenum = m_NumVertices;
	if (isClosed()) {
		singleSurfacenum = m_NumVertices + 1;
	}

	auto rowLen = singleSurfacenum + 1;

	m_VertexCoord->setNumOfTuple(rowLen+1);		//上下圆心
	int idx = 0;

	//上顶面 一个点
	for (int i = 0; i < 1; i++) {
		m_VertexCoord->setTuple(idx++, topPoints[i].x(), topPoints[i].y(), m_height * 0.5);
	}

	//下底面
	for (int i = 0; i < rowLen; i++) {
		m_VertexCoord->setTuple(idx++, bottomPoints[i].x(), bottomPoints[i].y(), -m_height * 0.5);
	}

	m_VertexCoord->Modified();
}

void XConeSource::updateVertextNormalArray()
{
}

void XConeSource::updateVertexColorArray()
{
}

void XConeSource::updateFaceIndexArray()
{
	if (isClosed()) {
		//如果闭合圆锥，则底面m_NumVertices个三角形，侧面m_NumVertices个三角形
		m_FaceIndexs->setNumOfTuple(m_NumVertices*2);
	}
	else {
		//如果不闭合，则底面m_NumVertices-1个三角形，侧面m_NumVertices+1个三角形
		m_FaceIndexs->setNumOfTuple(m_NumVertices * 2);
	}

	if (isClosed()) {
		//to, bo, p0,p1,p2,...pn,p0
		//先底面
		auto startVertexIdx = 1;//起始位置表示底面的中心
		auto triangleNum =0;
		for (int i = 0; i < m_NumVertices; i++) {
			m_FaceIndexs->setTuple(triangleNum++, startVertexIdx, startVertexIdx + 2 + i,startVertexIdx + 1 + i);
		}

		//再侧面
		for (int i = 0; i < m_NumVertices; i++) {
			auto ss = XQ::XAlgo::next_in_cycle(2 + i, 2, m_NumVertices + 2 - 1);
			m_FaceIndexs->setTuple(triangleNum++, 0/*顶点*/, 2 + i, XQ::XAlgo::next_in_cycle(2+i,2, m_NumVertices+2-1));
		}
	}
	else {
		//to bo, p0,p1,p2,...pn
		auto triangleNum =0;
		for (int i = 0; i < m_NumVertices-1; i++) {
			m_FaceIndexs->setTuple(triangleNum++, 1/*b0*/, 2+i+1,2+i);
		}

		//侧面
		for (int i = 0; i < m_NumVertices - 1; i++) {
			m_FaceIndexs->setTuple(triangleNum++, 0/*t0*/,  2 + i, XQ::XAlgo::next_in_cycle(2 + i, 2, m_NumVertices + 2 - 1));
		}

		//补全的两个面
		m_FaceIndexs->setTuple(triangleNum++, 0, 1, 2);
		m_FaceIndexs->setTuple(triangleNum++, 0, m_NumVertices+1, 1);
	}
	m_FaceIndexs->Modified();
}

void XConeSource::updateFaceColorArray()
{
	m_FaceColor->setNumOfTuple(m_NumVertices * 2);
	if (isClosed()) {
		//先底面 再侧面，各m_NumVertices个三角形
		for (int i = 0; i < m_NumVertices; i++) {
			m_FaceColor->setTuple(i,1,0,0,1);
		}

		for (int i = 0; i < m_NumVertices; i++) {
			m_FaceColor->setTuple(m_NumVertices +i, 0, 0, 1, 1);
		}
	}
	else {
		//先m_NumVertices-1个底面 再侧面m_NumVertices-1个三角形 最后2个侧面
		auto num =0;
		for (int i = 0; i < m_NumVertices - 1; i++) {
			m_FaceColor->setTuple(num++, 1, 0, 0, 1);
		}

		for (int i = 0; i < m_NumVertices - 1; i++) {
			m_FaceColor->setTuple(num++ , 0, 0, 1,1);
		}
		m_FaceColor->setTuple(num++, 1, 1, 1, 1);
		m_FaceColor->setTuple(num++, 1, 1, 1, 1);
	}
	m_FaceColor->Modified();
}

void XConeSource::updateLineIndexArray()
{
	auto lineNum =0;
	if (isClosed()) {
		lineNum = m_NumVertices*2;
		m_LineIndexs->setNumOfTuple(lineNum);
		for (int i = 0; i < m_NumVertices; i++) {
			m_LineIndexs->setTuple(i,2+i,XQ::XAlgo::next_in_cycle(2+i,2,m_NumVertices+2-1));
		}

		//侧边棱
		for (int i = 0; i < m_NumVertices; i++) {
			m_LineIndexs->setTuple(i+ m_NumVertices, 0, 2 + i);
		}
	}
	else {
		lineNum = m_NumVertices * 2 + 2;
		m_LineIndexs->setNumOfTuple(lineNum);
		for (int i = 0; i < m_NumVertices+1; i++) {
			m_LineIndexs->setTuple(i, 1 + i, XQ::XAlgo::next_in_cycle(1 + i, 1, m_NumVertices + 1));
		}

		for (int i = 0; i < m_NumVertices + 1; i++) {
			m_LineIndexs->setTuple(i+m_NumVertices+1, 0, 1+i);
		}
	}

	//先底面边 再侧面边
	m_LineIndexs->Modified();
}

void XConeSource::updateLineColorArray()
{
}
