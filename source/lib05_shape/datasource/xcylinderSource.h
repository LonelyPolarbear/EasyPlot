#pragma once
#include "../xshapeApi.h"
#include "xshapeSource.h"
#include <lib00_utilty/XUtilty.h>
#include <vector>
#include "xrotateLoftedGeometrySource.h"


class LIB05_SHAPE_API XCylinderSource : public XRotateLoftedgeometrySource {
protected:
	XCylinderSource();

	~XCylinderSource();
	virtual std::vector<XQ::Vec3f> getBottomPoints();
	virtual std::vector<XQ::Vec3f> getTopPoints();
public:
	void setNumVertices(unsigned int numVertices);
	void setAngle(double angle);
};