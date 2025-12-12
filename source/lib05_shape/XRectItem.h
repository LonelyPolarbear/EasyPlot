#pragma once
#include "XGraphicsItem.h"


class LIB05_SHAPE_API XRectItem :public XGraphicsItem {
public:
	enum RectType {
		center_rect,			// 矩形中心点和任意一个顶点
		corner_rect,		// 矩形任意两个顶点
		diagonal_rect		// 矩形任意两个对角顶点
	};
	XRectItem(std::shared_ptr<XGraphicsItem> parent = nullptr);
	virtual ~XRectItem();

	void setRectType(RectType rectType);
	RectType getRectType() const;

	void setRect(XQ::Vec2f pos1,XQ::Vec2f pos2);

protected:
	 void updateVboCoord()override;
	virtual uint32_t computeNumofVertices() override;
private:
	RectType m_rectType = RectType::diagonal_rect;

};