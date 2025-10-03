#pragma once
#include "xshapeApi.h"
#include "xshapeSource.h"
#include <lib00_utilty/myUtilty.h>
#include <vector>
#include "xrotateLoftedGeometrySource.h"


class LIB05_SHAPE_API XConeSource : public XRotateLoftedgeometrySource {
protected:
	XConeSource();

	~XConeSource();
	virtual std::vector<myUtilty::Vec3f> getBottomPoints();
	virtual std::vector<myUtilty::Vec3f> getTopPoints();
public:
	void setNumVertices(unsigned int numVertices);
	void setAngle(double angle);
};