#pragma once
#include "../xshapeApi.h"
#include "xshapeSource.h"
#include <lib00_utilty/myUtilty.h>
#include <vector>
#include "xrotateLoftedGeometrySource.h"


class LIB05_SHAPE_API XRegularPrimSource : public XRotateLoftedgeometrySource {
protected:
	XRegularPrimSource();

	~XRegularPrimSource();
	virtual std::vector<XQ::Vec3f> getBottomPoints();
	virtual std::vector<XQ::Vec3f> getTopPoints();
public:
	void setNumVertices(unsigned int numVertices);
	void setAngle(double angle);
};