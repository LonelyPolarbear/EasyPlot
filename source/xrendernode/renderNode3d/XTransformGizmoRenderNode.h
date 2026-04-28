#pragma once
#include <xrendernode/renderNode3d/XGroupRenderNode3d.h>
#include <xrendernode/renderNode3d/XArrowRenderNode.h>
#include <xrendernode/renderNode3d/XSphereRenderNode.h>
#include <xrendernode/renderNode3d/XTorusRenderNode.h>
#include <xrendernode/datasource/xsphereSource.h>
#include <Eigen/Eigen>

/// <summary>
/// 球节点
/// </summary>
class XTransformGizmoRenderNode : public XGroupRenderNode3d {
	REGISTER_CLASS_META_DATA(XTransformGizmoRenderNode, XGroupRenderNode3d);

	enum class InteractObjectType {
		none,
		origin,
		translate_x,
		translate_y,
		translate_z,
		rotate_x,
		rotate_y,
		rotate_z
	};
protected:
	XTransformGizmoRenderNode();
	virtual ~XTransformGizmoRenderNode();
public:
	void Init() override;
	void draw(const Eigen::Matrix4f& parentMatrix, bool isNormal) override;
	XQ::BoundBox getThisBoundBox(const Eigen::Matrix4f& m) const override;

	/**
	 * @brief 获取不同交互类型对应的rendernode
	 * param[in] InteractObjectType 交互类型，平移、旋转
	 */
	sptr<XRenderNode> getInteractObject(InteractObjectType type);

	/**
	 * @brief 判断一个renderNode是否是一个交互node
	 */
	InteractObjectType getInteractObjectType(sptr<XRenderNode>);

	/**
	 * @breif 绑定要操作的节点
	 */
	void bindRenderNode(sptr<XRenderNode> node);

	sptr<XRenderNode> getBindRenderNode();

	void notifySigMatrixChanged();
public:
	XSIGNAL(void(const Eigen::Matrix4f&)) SigMatrixChanged;
protected:
	class Internal;
	std::unique_ptr<Internal> mData;
};