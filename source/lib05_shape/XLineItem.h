#pragma once
#include "XGraphicsItem.h"


class LIB05_SHAPE_API XLineItem :public XGraphicsItem {
public:
	XLineItem(std::shared_ptr<XGraphicsItem> parent);
	virtual ~XLineItem();
	void updateVboCoord() override;
	void setLine(const myUtilty::Vec2f& start, const myUtilty::Vec2f& end);
	myUtilty::Vec2f getStart() const;
	myUtilty::Vec2f getEnd() const;

	double getLength() const;
	virtual uint32_t computeNumofVertices() override;
};