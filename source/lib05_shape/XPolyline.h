#pragma once
#include "XGraphicsItem.h"


class LIB05_SHAPE_API XPolyline :public XGraphicsItem {
public:
	XPolyline();
	virtual ~XPolyline();
	virtual void updateData();

	virtual uint32_t computeNumofVertices() override;
};