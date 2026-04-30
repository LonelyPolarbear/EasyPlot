#pragma once
#include "../xrendernodeApi.h"
#include "xshapeSource.h"
#include <lib00_utilty/XUtilty.h>
#include <vector>
#include "xrotateLoftedGeometrySource.h"


class XRENDERNODE_API XRegularPrimSource : public XRotateLoftedgeometrySource {
protected:
	XRegularPrimSource();

	~XRegularPrimSource();
	virtual std::vector<XQ::Vec3f> getBottomPoints();
	virtual std::vector<XQ::Vec3f> getTopPoints();
	void updateLineIndexArray() override;
	void updateVertexIndexArray() override;
public:
	void setNumVertices(unsigned int numVertices);
	void setAngle(double angle);
};