#pragma once
#include "XGraphicsItem.h"
#include "XTextItem.h"


class LIB05_SHAPE_API XScreenTextItem :public XTextItem {
public:
	
	XScreenTextItem();
	virtual ~XScreenTextItem();

	void setTextScreenPos(int x, int y);
	 myUtilty::Vec2i getTextScrrenPos();
	myUtilty::Vec2i m_screenPos;
};