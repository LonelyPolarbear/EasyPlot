#pragma once
#include "../XGraphicsItem.h"


class XRENDERNODE_API XPolyline :public XGraphicsItem {
public:
	XPolyline(std::shared_ptr<XGraphicsItem> parent);
	virtual ~XPolyline();
	void updateVboCoord() override;
	virtual uint32_t computeNumofVertices() override;
	virtual void pickBorderDraw(std::shared_ptr<xshader> shader, const Eigen::Matrix4f& m);
	virtual void pickFillDraw(std::shared_ptr<xshader> shader, const Eigen::Matrix4f& m);
};