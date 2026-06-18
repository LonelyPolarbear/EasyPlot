#pragma once
#include "base/xbaserender/xbaserenderApi.h"
#include "base/xbaserender/baseRender/XRenderType.h"
#include "base/xbaserender/baseRender/XBaseInteractionEventHandler.h"
#include <dataBase/XDataBaseObject.h>
#include <dataBase/XDataObject.h>
#include <dataBase/XDataList.h>
#include <xsignal/XSignal.h>
#include <lib00_utilty/XUtilty.h>
/**
 * @class XBaseRenderNode
 * @brief 基础渲染节点
 */
class xShaderManger;
class XBaseRender;
class XBaseRenderTexture;

class XBASERENDER_API XBaseRenderNodeState : public XDataObject {
	REGISTER_CLASS_META_DATA(XBaseRenderNodeState, XDataObject);
protected:
	XBaseRenderNodeState();
	virtual ~XBaseRenderNodeState();
};


class XBASERENDER_API XBaseRenderNodeExtInterface {
public:
	virtual void LeftButtonPressEvent(sptr<XBaseRender>, XQ::Vec2i,XQ::KeyboardModifier, XEvent& event, XQ::Vec3f fragcoord) {}
	virtual void LeftButtonReleaseEvent(sptr<XBaseRender>, XQ::Vec2i, XQ::KeyboardModifier, XEvent& event, XQ::Vec3f fragcoord) {}
	virtual void MiddleButtonPressEvent(sptr<XBaseRender>, XQ::Vec2i, XQ::KeyboardModifier, XEvent& event, XQ::Vec3f fragcoord) {}
	virtual void MiddleButtonReleaseEvent(sptr<XBaseRender>, XQ::Vec2i, XQ::KeyboardModifier, XEvent& event, XQ::Vec3f fragcoord) {}
	virtual void RightButtonPressEvent(sptr<XBaseRender>, XQ::Vec2i, XQ::KeyboardModifier, XEvent& event, XQ::Vec3f fragcoord) {}
	virtual void RightButtonReleaseEvent(sptr<XBaseRender>, XQ::Vec2i, XQ::KeyboardModifier, XEvent& event, XQ::Vec3f fragcoord) {}
	virtual void LeftButtonDoublePressEvent(sptr<XBaseRender>, XQ::Vec2i, XQ::KeyboardModifier, XEvent& event, XQ::Vec3f fragcoord) {}
	virtual void RightButtonDoublePressEvent(sptr<XBaseRender>, XQ::Vec2i, XQ::KeyboardModifier, XEvent& event, XQ::Vec3f fragcoord) {}
	virtual void MiddleButtonDoublePressEvent(sptr<XBaseRender>, XQ::Vec2i, XQ::KeyboardModifier, XEvent& event, XQ::Vec3f fragcoord) {}
	virtual void EnterEvent(sptr<XBaseRender>, XEvent& event, XQ::Vec3f fragcoord) {}
	virtual void LeaveEvent(sptr<XBaseRender>, XEvent& event, XQ::Vec3f fragcoord) {}
	virtual void FoucsInEvent(sptr<XBaseRender>, XEvent& event, XQ::Vec3f fragcoord) {}
	virtual void FoucsOutEvent(sptr<XBaseRender>, XEvent& event, XQ::Vec3f fragcoord) {}
	virtual void ResizeEvent(sptr<XBaseRender>, XQ::Vec2i, XEvent& event, XQ::Vec3f fragcoord) {}
	virtual void KeyPressEvent(sptr<XBaseRender>, XQ::Key, XQ::KeyboardModifier, XEvent& event, XQ::Vec3f fragcoord) {}
	virtual void KeyReleaseEvent(sptr<XBaseRender>, XQ::Key, XQ::KeyboardModifier, XEvent& event, XQ::Vec3f fragcoord) {}
	virtual void MouseMoveEvent(sptr<XBaseRender>, XQ::Vec2i, XQ::KeyboardModifier, XEvent& event, XQ::Vec3f fragcoord) {}
	virtual void MouseWheelForwardEvent(sptr<XBaseRender>, XQ::Vec2i, XQ::KeyboardModifier, XEvent& event, XQ::Vec3f fragcoord) {}
	virtual void MouseWheelBackwardEvent(sptr<XBaseRender>, XQ::Vec2i, XQ::KeyboardModifier, XEvent& event, XQ::Vec3f fragcoord) {}
};

class XBASERENDER_API XBaseRenderNode : public XDataObject {
	REGISTER_CLASS_META_DATA(XBaseRenderNode, XDataObject);
protected:
	XBaseRenderNode();
	virtual ~XBaseRenderNode();
public:
		/**
		 * @brief 节点绘制
		 * @param render 当前调用的render
		 * @param parentMatrix 当前节点的父矩阵，为累积矩阵，如果没有父，则为单位矩阵
		 */
		virtual void draw(sptr<XBaseRender>  render,const Eigen::Matrix4f& parentMatrix) = 0;

		/**
		 * @brief 添加子渲染节点 
		 */
		virtual bool addChildRenderNode(sptr<XBaseRenderNode> child) =0;

		/**
		 * @brief 移除子渲染节点
		 */
		virtual bool removeChildRenderNode(sptr< XBaseRenderNode> child) =0;

		/**
		 * @brief 移除所有子节点
		 */
		virtual void clearChildrenRenderNode() =0;

		/**
		 * @brief 获取子节点数量
		 */
		virtual int getChildRenderNodeCount() const =0;

		/**
		 * @brief 获取指定位置的子节点
		 * @param index 节点位置，若位置无效，则返回nullptr
		 * @return 子节点
		 */
		virtual sptr<XBaseRenderNode> getChildRenderNode(int index) const =0;

		/**
		 * @brief 获取父节点
		 */
		virtual sptr<XBaseRenderNode> getRenderNodeParent() const =0;

		/**
		 * @brief 设置节点可见性
		 * @param visible=true,表示节点可见，false表示不可见(即不参与渲染)
		 */
		virtual void setVisible(bool visible) =0;

		/**
		 * @brief 获取节点可见性
		 */
		virtual bool isVisible() =0;

		/**
		 * @brief 设置节点的着色器管理器，用于渲染时候，获取当前节点使用的着色器
		 */
		virtual void setShaderManger(std::shared_ptr<xShaderManger> shaderManger)=0;

		/**
		 * @brief 为当前节点添加纹理，若纹理已经存在，返回false
		 */
		virtual bool addRenderTexture(sptr<XBaseRenderTexture> texture) = 0;

		/**
		 * @brief 当前节点包含的纹理数量
		 */
		virtual int getRenderTextureNum() const = 0;

		/**
		 * @brief 获取指定索引的纹理，若index无效，则返回nullptr
		 */
		virtual sptr<XBaseRenderTexture> getRenderTexture(int idx) const = 0;

		/**
		 * @brief 获取着色器管理器，若未设置，则递归返回父节点的着色器管理器
		 */
		virtual std::shared_ptr<xShaderManger> getShaderManger() const=0;

		/**
		 * @brief 获取当前节点的包围盒(包括子节点)
		 * @param m 当前节点叠加的父类变换矩阵，包围盒会考虑该矩阵
		 */
		virtual XQ::BoundBox getBoundBox(const Eigen::Matrix4f& m) const = 0;

		/**
		 * @brief 获取当前节点的包围盒(不包括子节点)
		 * @param m 当前节点叠加的父类变换矩阵，包围盒会考虑该矩阵
		 */
		virtual XQ::BoundBox getThisBoundBox(const Eigen::Matrix4f& m) const = 0;

		/**
		 * @brief 获取当前节点的模型变换矩阵
		 */
		virtual Eigen::Affine3f getTransform() const = 0;

		/**
		 * @brief 获取当前节点的父矩阵(累积矩阵)
		 */
		virtual void getChainTransform(Eigen::Affine3f& afiine) const = 0;

		/**
		 * @brief 设置当前节点的模型变换矩阵
		 */
		virtual void setTransform(const Eigen::Affine3f& transform) = 0;

		/**
		 * @brief 充值当前节点的模型变换矩阵为单位矩阵
		 */
		virtual void resetTransform() = 0;

		/**
		 * @brief 节点平移，节点沿着自身坐标系平移
		 */
		virtual void translate(float x, float y, float z) = 0;

		/**
		 * @brief 设置节点位置，直接修改矩阵的平移分量
		 */
		virtual void setPosition(float x, float y, float z) = 0;

		/**
		 * @brief 设置节点缩放，直接修改缩放矩阵
		 */
		virtual void setScale(float x, float y, float z) = 0;

		/**
		 * @brief 节点旋转，节点沿着自身坐标系旋转
		 */
		virtual void rotate(float angle, XQ::Vec3f dir) = 0;

		/**
		 * @brief 旋转变换 angle角度，不是弧度
		 */
		virtual void rotateX(float angle) = 0;
		virtual void rotateY(float angle) = 0;
		virtual void rotateZ(float angle) = 0;
		virtual void scale(float x, float y, float z) = 0;

		/**
		 * @brief 获取节点id，全局唯一
		 */
		virtual int64_t getID() const =0;

		/**
		 * @brief 设置节点状态
		 */
		virtual void setNodeState(sptr<XBaseRenderNodeState>) = 0;

		/**
		 * @brief 获取节点状态
		 */
		virtual sptr<XBaseRenderNodeState> getNodeState() const = 0;

		sptr<XBaseRenderNodeExtInterface> getExtInterface();
};

