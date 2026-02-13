#include "xregularPrimSource.h"
#include <dataBase/XVector.h>

XRegularPrimSource::XRegularPrimSource()
{
}

XRegularPrimSource::~XRegularPrimSource()
{
}


void XRegularPrimSource::setNumVertices(unsigned int numVertices)
{
	XRotateLoftedgeometrySource::setNumVertices(numVertices);
}

void XRegularPrimSource::setAngle(double angle)
{
	XRotateLoftedgeometrySource::setAngle(angle);
}

std::vector<XQ::Vec3f> XRegularPrimSource::getBottomPoints() {
	std::vector<XQ::Vec3f> result;
	std::vector<XQ::Vec2f> tmp;
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

std::vector<XQ::Vec3f> XRegularPrimSource::getTopPoints() {
	std::vector<XQ::Vec3f> result;
	std::vector<XQ::Vec2f> tmp;
	if (isClosed()) {
		tmp = XQ::math::circleSplit(m_NumVertices, m_angle);
	}
	else {
		tmp = XQ::math::circleSplit(m_NumVertices - 1, m_angle);
	}

	result.resize(tmp.size() + 1);
	result[0] = XQ::Vec3f(0, 0, m_height * 0.5);
	for (int i = 0; i < tmp.size(); i++) {
		result[i + 1] = XQ::Vec3f(tmp[i].x(), tmp[i].y(), m_height * 0.5);
	}

	return result;
}

void XRegularPrimSource::updateLineIndexArray()
{
	auto bottomPoints = getBottomPoints();
	if (bottomPoints.size() == 0)
		return;

	//如果是闭合，则内部的中心点不需要连线，一共m_NumVertices个点
	//顶面中心 0 1 2 3 4 ... n 0   //m_NumVertices+2
	//底面中心 0 1 2 3 4 ... n 0   //m_NumVertices+2
	if (isClosed()) {
		auto lineNum = m_NumVertices * 2 + m_NumVertices;//上下面+棱
		m_LineIndexs->setNumOfTuple(lineNum);

		for (int i = 0; i < m_NumVertices; i++) {
			m_LineIndexs->setTuple(i, i + 1, i + 2);
		}

		int createdLineNum = m_NumVertices;
		int startVertexIdx = m_NumVertices + 2;

		/*for (int i = 0; i < m_NumVertices; i++) {
			m_LineIndexs->setTuple(i + createdLineNum, i + 1, i + 2);
		}*/

		for (int i = 0; i < m_NumVertices; i++) {
			m_LineIndexs->setTuple(i + createdLineNum, startVertexIdx + i + 1, startVertexIdx + i + 2);
		}

		//棱
		createdLineNum += m_NumVertices;
		for (int i = 0; i < m_NumVertices; i++) {
			m_LineIndexs->setTuple(i + createdLineNum, i + 1, i + 1 + m_NumVertices + 2);
		}
		m_LineIndexs->Modified();
	}
	else {
		//顶面中心 0 1 2 3 4 ... n   //m_NumVertices+1
		//底面中心 0 1 2 3 4 ... n   //m_NumVertices+1
		//不闭合，此时直线需要连接中心点
		//每个面一共m_NumVertices+1条线 侧边棱m_NumVertices+1
		auto lineNum = (m_NumVertices + 1) * 2 + m_NumVertices + 1;
		m_LineIndexs->setNumOfTuple(lineNum);

		//顶面的边
		for (int i = 0; i < m_NumVertices + 1; i++) {
			m_LineIndexs->setTuple(i, i, (i + 1) % (m_NumVertices + 1));
		}

		int createdLineNum = m_NumVertices + 1;
		int startVertexIdx = m_NumVertices + 1;

		for (int i = 0; i < m_NumVertices + 1; i++) {
			m_LineIndexs->setTuple(i + createdLineNum, i+ startVertexIdx, (i + 1) % (m_NumVertices + 1) + startVertexIdx);
		}

		//侧棱
		createdLineNum += m_NumVertices + 1;
		for (int i = 0; i < m_NumVertices+1; i++) {
			m_LineIndexs->setTuple(i + createdLineNum, i, startVertexIdx+i);
		}
		m_LineIndexs->Modified();
	}
}

void XRegularPrimSource::updateVertexIndexArray()
{
	auto bottomPoints = getBottomPoints();
	if (bottomPoints.size() == 0)
		return;

	//如果是闭合，则内部的中心点不需要，一共m_NumVertices个点
	//顶面中心 0 1 2 3 4 ... n 0   //m_NumVertices+2
	//底面中心 0 1 2 3 4 ... n 0   //m_NumVertices+2
	if (isClosed()) {
		auto pointNum = m_NumVertices * 2;//上下面
		auto startVertexIdx = 1;
		m_VertexIndexs->setNumOfTuple(pointNum);
		auto createPointNum =0;
		for (int i = 0; i < m_NumVertices ; i++) {
			m_VertexIndexs->setTuple(i + createPointNum, i + startVertexIdx);
		}

		createPointNum +=m_NumVertices;
		startVertexIdx +=m_NumVertices;
		for (int i = 0; i < m_NumVertices; i++) {
			m_VertexIndexs->setTuple(i + createPointNum, i + startVertexIdx);
		}

		m_VertexIndexs->Modified();
	}
	else {
		//顶面中心 0 1 2 3 4 ... n   //m_NumVertices+1
		//底面中心 0 1 2 3 4 ... n   //m_NumVertices+1
		//不闭合，此时需要中心点
		//每个面一共m_NumVertices+1个点
		auto pointNum = (m_NumVertices+1) * 2;//上下面
		auto startVertexIdx = 0;
		m_VertexIndexs->setNumOfTuple(pointNum);
		auto createPointNum = 0;
		for (int i = 0; i < m_NumVertices + 1; i++) {
			m_VertexIndexs->setTuple(i + createPointNum, i + startVertexIdx);
		}

		createPointNum += m_NumVertices+1;
		startVertexIdx += m_NumVertices+1;
		for (int i = 0; i < m_NumVertices + 1; i++) {
			m_VertexIndexs->setTuple(i + createPointNum, i + startVertexIdx);
		}

		m_VertexIndexs->Modified();
	}
}
