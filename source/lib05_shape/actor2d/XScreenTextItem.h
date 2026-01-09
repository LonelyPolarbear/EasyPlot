#pragma once
#include "../XGraphicsItem.h"
#include "XTextItem.h"


class LIB05_SHAPE_API XScreenTextItem :public XTextItem {
public:
	
	XScreenTextItem(std::shared_ptr<XGraphicsItem> parent);
	virtual ~XScreenTextItem();

	void setTextSceneScreenPos(int x, int y);
	
private:
	XQ::Vec2i m_scene_screenPos;
};