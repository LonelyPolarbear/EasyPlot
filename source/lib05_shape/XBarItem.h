#pragma once
#include "XGraphicsItem.h"


class LIB05_SHAPE_API XBarItem :public XGraphicsItem {
public:
	XBarItem(std::shared_ptr<XGraphicsItem> parent);
	virtual ~XBarItem();
	virtual void updateData();
	virtual void drawBorder(std::shared_ptr<xshader> shader, const Eigen::Matrix4f& m){}

	virtual void drawFill(std::shared_ptr<xshader> shader, const Eigen::Matrix4f& m);
	virtual uint32_t computeNumofVertices() override;

protected:
	virtual void initResource();
	std::shared_ptr< XFloatArray> m_data;
};