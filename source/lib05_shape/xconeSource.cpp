#include "xconeSource.h"

XConeSource::XConeSource()
{
	m_NumVertices = 360;
	m_angle = 360;
}

XConeSource::~XConeSource()
{
}


void XConeSource::setNumVertices(unsigned int numVertices)
{
	XRotateLoftedgeometrySource::setNumVertices(numVertices);
}

void XConeSource::setAngle(double angle)
{
	XRotateLoftedgeometrySource::setAngle(angle);
}

std::vector<myUtilty::Vec3f> XConeSource::getBottomPoints() {
	std::vector<myUtilty::Vec3f> result;
	std::vector<myUtilty::Vec2f> tmp;
	if (isClosed()) {
		tmp = myUtilty::math::circleSplit(m_NumVertices, m_angle);
	}
	else {
		tmp = myUtilty::math::circleSplit(m_NumVertices - 1, m_angle);
	}

	result.resize(tmp.size() + 1);
	result[0] = myUtilty::Vec3f(0, 0, -m_height * 0.5);
	for (int i = 0; i < tmp.size(); i++) {
		result[i + 1] = myUtilty::Vec3f(tmp[i].x(), tmp[i].y(), -m_height * 0.5);
	}

	return result;
}

std::vector<myUtilty::Vec3f> XConeSource::getTopPoints() {
	std::vector<myUtilty::Vec3f> result;
	std::vector<myUtilty::Vec2f> tmp;
	if (isClosed()) {
		tmp = myUtilty::math::circleSplit(m_NumVertices, m_angle);
	}
	else {
		tmp = myUtilty::math::circleSplit(m_NumVertices - 1, m_angle);
	}

	result.resize(tmp.size() + 1);
	result[0] = myUtilty::Vec3f(0, 0, m_height * 0.5);
	for (int i = 0; i < tmp.size(); i++) {
		result[i + 1] = myUtilty::Vec3f(0, 0, m_height * 0.5);
	}

	return result;
}