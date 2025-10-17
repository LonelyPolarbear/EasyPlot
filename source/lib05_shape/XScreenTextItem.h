#pragma once
#include "XGraphicsItem.h"
#include "XTextItem.h"


class LIB05_SHAPE_API XScreenTextItem :public XTextItem {
public:
	
	XScreenTextItem();
	virtual ~XScreenTextItem();

	void setTextSceneScreenPos(int x, int y);
	
	myUtilty::Vec2i getTextSceneScreenPos() const;
private:
	myUtilty::Vec2i m_scene_screenPos;

	
};