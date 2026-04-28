#pragma once
#include "../XGraphicsItem.h"


class xrendernode_API XCompositeItem :public XGraphicsItem {
public:
	XCompositeItem(std::shared_ptr<XGraphicsItem> parent = nullptr);
	virtual ~XCompositeItem();	

	virtual void draw(const Eigen::Matrix4f& m = Eigen::Matrix4f::Identity());
	virtual void drawFill(std::shared_ptr<xshader> fill_shader, const Eigen::Matrix4f& m){}
	virtual void drawBorderImpl(std::shared_ptr<xshader> border_shader, const Eigen::Matrix4f& m, bool){}
};