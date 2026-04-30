#pragma once
#include "XCompositeItem.h"

class XLineItem;
class XTextItem;
class XRENDERNODE_API XAxisItem :public XCompositeItem {
public:
	XAxisItem(std::shared_ptr<XGraphicsItem> parent = nullptr);
	virtual ~XAxisItem();
	virtual void updateData();
	
	void setRange(double minVal,double maxVal);
	std::shared_ptr<XLineItem> getLine() const {
		return m_line; 
	}
	XQ::Vec2d getRange() const {
		return mRange;
	}

	void updateTextPos();

	void setLabelNum(int num);

	void setLayout(XGL::Layout layout);
protected:
	std::shared_ptr<XLineItem> m_line;
	std::vector<std::shared_ptr<XTextItem>> m_texts;
	int mLabelNum = 5;
	XGL::Layout mLayout = XGL::Layout::horizontal;
	XQ::Vec2d mRange = XQ::Vec2d(-1, 1);		//ÖáµÄ·¶Î§
};