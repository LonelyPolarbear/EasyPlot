#pragma once
#include "../XGraphicsItem.h"


class LIB05_SHAPE_API XTransformItem :public XGraphicsItem {
public:
	XTransformItem(std::shared_ptr<XGraphicsItem> parent = nullptr);
	virtual ~XTransformItem();	

	virtual void draw(const Eigen::Matrix4f& m = Eigen::Matrix4f::Identity());
	virtual void drawFill(std::shared_ptr<xshader> fill_shader, const Eigen::Matrix4f& m);
	virtual void drawBorderImpl(std::shared_ptr<xshader> border_shader, const Eigen::Matrix4f& m, bool);
	virtual void pickFillDraw(std::shared_ptr<xshader> shader, const Eigen::Matrix4f& m);
	virtual void pickBorderDraw(std::shared_ptr<xshader> shader, const Eigen::Matrix4f& m);
};