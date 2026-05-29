#pragma once
#include <xrendernode/renderNode3d/XGroupRenderNode3d.h>
#include <xrendernode/datasource/xCustomSource.h>
#include <Eigen/Eigen>

/// <summary>
/// ¾ØÐÎäÖÈ¾½Úµã
/// </summary>
class XCustomGroupRenderNode3d : public XGroupRenderNode3d {
	REGISTER_CLASS_META_DATA(XCustomGroupRenderNode3d, XGroupRenderNode);
protected:
	XCustomGroupRenderNode3d();
	virtual ~XCustomGroupRenderNode3d();
public:
	void Init() override;
	void draw(sptr<XBaseRender> render, const Eigen::Matrix4f& parentMatrix) override;
};