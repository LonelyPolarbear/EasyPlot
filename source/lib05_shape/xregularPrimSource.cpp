#include "xregularPrimSource.h"

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

std::vector<myUtilty::Vec3f> XRegularPrimSource::getBottomPoints() {
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
		result[i + 1] = myUtilty::Vec3f(tmp[i].x, tmp[i].y, -m_height * 0.5);
	}

	return result;
}

std::vector<myUtilty::Vec3f> XRegularPrimSource::getTopPoints() {
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
		result[i + 1] = myUtilty::Vec3f(tmp[i].x, tmp[i].y, m_height * 0.5);
	}

	return result;
}