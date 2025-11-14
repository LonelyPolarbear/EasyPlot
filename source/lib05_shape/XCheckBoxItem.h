#pragma once
#include "XGraphicsItem.h"
#include "XRectItem.h"
class LIB05_SHAPE_API XCheckBoxItem :public XRectItem {
public:
	XCheckBoxItem(std::shared_ptr<XGraphicsItem> parent = nullptr);
	virtual ~XCheckBoxItem();

	void setChecked(bool checked);
	bool isChecked() const;
protected:
	class Internal;
	std::shared_ptr<Internal> m_internal;
};