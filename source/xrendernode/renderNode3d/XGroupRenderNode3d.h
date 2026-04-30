#pragma once
#include <xrendernode/XRenderNode.h>
#include <xrendernode/datasource/xCustomSource.h>
#include <Eigen/Eigen>

/// <summary>
/// ¾ØÐÎäÖÈ¾½Úµã
/// </summary>
class XGroupRenderNode3d : public XGroupRenderNode {
	REGISTER_CLASS_META_DATA(XGroupRenderNode3d, XGroupRenderNode);
protected:
	XGroupRenderNode3d();
	virtual ~XGroupRenderNode3d();
public:
	void Init() override;
	void draw(sptr<XBaseRender> render, const Eigen::Matrix4f& parentMatrix, bool isNormal) override;
	void drawInstance(sptr<XBaseRender> render, const Eigen::Matrix4f& parentMatrix) override{}
};