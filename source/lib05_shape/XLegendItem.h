#pragma once
#include "XGraphicsItem.h"
#include "XCompositeItem.h"
#include "XRectItem.h"
class LIB05_SHAPE_API XLegendItem :public XRectItem {
public:
	XLegendItem(std::shared_ptr<XGraphicsItem> parent = nullptr);
	virtual ~XLegendItem();

	bool addCurve(std::shared_ptr <XGraphicsItem> curve);
protected:
	void updateChildPosition(const Eigen::Matrix4f& m) override;
protected:
	class Internal;
	std::shared_ptr<Internal> m_internal;
};