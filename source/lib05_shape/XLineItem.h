#pragma once
#include "XGraphicsItem.h"


class LIB05_SHAPE_API XLineItem :public XGraphicsItem {
public:
	XLineItem();
	virtual ~XLineItem();
	virtual void updateData();

	void setLine(const myUtilty::Vec2f& start, const myUtilty::Vec2f& end);
	virtual uint32_t computeNumofVertices() override;
};