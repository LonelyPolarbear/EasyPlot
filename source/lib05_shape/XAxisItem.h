#pragma once
#include "XGraphicsItem.h"

class XLineItem;
class XTextItem;
class LIB05_SHAPE_API XAxisItem :public XGraphicsItem {
public:
	XAxisItem();
	virtual ~XAxisItem();
	virtual void updateData();
	virtual void initResource();
	void setRange(double minVal,double maxVal);
	std::shared_ptr<XLineItem> getLine() const { return m_line; }
protected:
	std::shared_ptr<XLineItem> m_line;
	std::vector<std::shared_ptr<XTextItem>> m_texts;
	int mLabelNum = 5;
	XGL::Layout mLayout = XGL::Layout::horizontal;
	myUtilty::Vec2d mRange;		//ÖáµÄ·¶Î§
};