#pragma once
#include "XGraphicsItem.h"


class LIB05_SHAPE_API XPolyline :public XGraphicsItem {
public:
	XPolyline();
	virtual ~XPolyline();
	void updateVboCoord() override;
	virtual uint32_t computeNumofVertices() override;
	virtual void pickBorderDraw(std::shared_ptr<xshader> shader, const Eigen::Matrix4f& m);
	virtual void pickFillDraw(std::shared_ptr<xshader> shader, const Eigen::Matrix4f& m);
};