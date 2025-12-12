#include "xcylinderSource.h"

XCylinderSource::XCylinderSource()
{
	m_angle = 360;
	m_NumVertices = 360;
	Modified();
}

XCylinderSource::~XCylinderSource()
{
}


void XCylinderSource::setNumVertices(unsigned int numVertices)
{
	XRotateLoftedgeometrySource::setNumVertices(numVertices);
}

void XCylinderSource::setAngle(double angle)
{
	XRotateLoftedgeometrySource::setAngle(angle);
}

std::vector<XQ::Vec3f> XCylinderSource::getBottomPoints() {
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

std::vector<XQ::Vec3f> XCylinderSource::getTopPoints() {
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
		result[i + 1] = XQ::Vec3f(0, 0, m_height * 0.5);
	}

	return result;
}