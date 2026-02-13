#pragma once
#include <lib05_shape/XRenderNode.h>
#include <lib05_shape/datasource/xCustomSource.h>
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
	void draw(const Eigen::Matrix4f& parentMatrix, bool isNormal) override;
	void drawInstance(const Eigen::Matrix4f& parentMatrix) override{}
};