#pragma once
#include <lib05_shape/renderNode3d/XGroupRenderNode3d.h>
#include <lib05_shape/renderNode3d/XArrowRenderNode.h>
#include <lib05_shape/renderNode3d/XSphereRenderNode.h>
#include <lib05_shape/renderNode3d/XTorusRenderNode.h>
#include <lib05_shape/datasource/xsphereSource.h>
#include <Eigen/Eigen>

/// <summary>
/// 球节点
/// </summary>
class XTransformGizmoRenderNode : public XGroupRenderNode3d {
	REGISTER_CLASS_META_DATA(XTransformGizmoRenderNode, XGroupRenderNode3d);
protected:
	XTransformGizmoRenderNode();
	virtual ~XTransformGizmoRenderNode();
public:
	void Init() override;
	void draw(const Eigen::Matrix4f& parentMatrix, bool isNormal) override;
protected:
	//操作柄由几部分几何组成的装配体 三个箭头、三个圆环,中心点是一个球体
	sptr<XArrowRenderNode> mArrowX;
	sptr<XArrowRenderNode> mArrowY;
	sptr<XArrowRenderNode> mArrowZ;
	sptr<XSphereRenderNode> mSphere;

	sptr< XTorusRenderNode> mRotateXY;
	sptr< XTorusRenderNode> mRotateYZ;
	sptr< XTorusRenderNode> mRotateZX;
};