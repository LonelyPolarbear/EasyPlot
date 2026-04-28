#pragma once
#include "../xrendernodeApi.h"
#include "xshapeSource.h"
#include <lib00_utilty/XUtilty.h>
#include <vector>
#include "xrotateLoftedGeometrySource.h"


class xrendernode_API XCylinderSource : public XRotateLoftedgeometrySource {
protected:
	XCylinderSource();

	~XCylinderSource();
	virtual std::vector<XQ::Vec3f> getBottomPoints();
	virtual std::vector<XQ::Vec3f> getTopPoints();
public:
	void setNumVertices(unsigned int numVertices);
	void setAngle(double angle);
};