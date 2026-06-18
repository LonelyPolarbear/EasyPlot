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
		rotate_z,
		scale_x,
		scale_y,
		scale_z
	};
protected:
	XTransformGizmoRenderNode();
	virtual ~XTransformGizmoRenderNode();
public:
	void Init() override;
	void draw(sptr<XBaseRender> render, const Eigen::Matrix4f& parentMatrix) override;
	XQ::BoundBox getThisBoundBox(const Eigen::Matrix4f& m) const override;

	void reset();
	/**
	 * @brief 获取不同交互类型对应的rendernode
	 * param[in] InteractObjectType 交互类型，平移、旋转
	 */
	sptr<XRenderNode> getInteractObject(InteractObjectType type);

	/**
	 * @brief 判断一个renderNode是否是一个交互node
	 */
	InteractObjectType getInteractObjectType(sptr<XRenderNode>,XQ::KeyboardModifier m);

	/**
	 * @breif 绑定要操作的节点
	 */
	void bindRenderNode(sptr<XRenderNode> node);

	sptr<XRenderNode> getBindRenderNode();

	void notifySigMatrixChanged( const XQ::Vec3f& scaleIncrement= XQ::Vec3f(1,1,1));
public:
	XSIGNAL(void(const Eigen::Matrix4f&,const XQ::Vec3f& scaleIncrement)) SigMatrixChanged;
	csptr<XAttr_Vec2f> AttrArrowSize;		//宽 高
	csptr<XAttr_Float> AttrXLen;				//X方向长度
	csptr<XAttr_Float> AttrYLen;				//Y方向长度
	csptr<XAttr_Float> AttrZLen;				//Z方向长度
	csptr<XAttr_Float> AttrLineRdius;		//线的半径

	csptr<XAttr_Float> AttrFixLenx;			//屏幕固定大小
	csptr<XAttr_Float> AttrFixLeny;			//屏幕固定大小
	csptr<XAttr_Float> AttrFixLenz;			//屏幕固定大小
	csptr<XAttr_Float> AttrFixRadius;			//屏幕固定大小
protected:
	class Internal;
	std::unique_ptr<Internal> mData;
};