#pragma once
#include "../XGraphicsItem.h"
#include "XCompositeItem.h"
#include "XRectItem.h"
class LIB05_SHAPE_API XLegendRowItem :public XRectItem {
public:
	XLegendRowItem(std::shared_ptr<XGraphicsItem> parent = nullptr);
	virtual ~XLegendRowItem();

	void setChecked(bool checked);
	bool isChecked() const;
	void setCurve(const std::shared_ptr<XGraphicsItem>& curve);
	void draw(const Eigen::Matrix4f& m) override;
	double getWidth() const ;
	double getHeight() const ;
protected:
	void updateChildPosition(const Eigen::Matrix4f& m) override;
protected:
	class Internal;
	std::shared_ptr<Internal> m_internal;
};