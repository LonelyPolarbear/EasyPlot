#pragma once
#include "xrendernodeApi.h"
#include <dataBase/XDataBaseObject.h>
#include <dataBase/XDataObject.h>
#include <dataBase/XDataList.h>
#include <xsignal/XSignal.h>
#include <lib00_utilty/XUtilty.h>

#include "XRenderNodeAttribute.h"
#include <base/xbaserender/baseNode/XBaseRenderNode.h>
#include <Eigen/Eigen>

class xShaderManger;
class XRENDERNODE_API XRenderNode :public XBaseRenderNode {
	REGISTER_CLASS_META_DATA(XRenderNode, XBaseRenderNode);
public:
	void Init() override;

	bool addChildRenderNode(sptr<XBaseRenderNode> child)override;
	bool removeChildRenderNode(sptr< XBaseRenderNode> child)override;
	void clearChildrenRenderNode()override;
	int getChildRenderNodeCount() const override;
	sptr<XBaseRenderNode> getChildRenderNode(int index) const override;
	sptr<XBaseRenderNode> getRenderNodeParent() const override;

	
	virtual void setVisible(bool visible) override;

	virtual bool isVisible() override ;

	void setShaderManger(std::shared_ptr<xShaderManger> shaderManger) override;

	std::shared_ptr<xShaderManger> getShaderManger() const override;

	//公共的虚口
	virtual XQ::BoundBox getBoundBox(const Eigen::Matrix4f& m) const override;

	virtual XQ::BoundBox getThisBoundBox(const Eigen::Matrix4f& m) const  override { return {}; };

	Eigen::Affine3f getTransform() const override;

	void getChainTransform(Eigen::Affine3f& afiine) const override;

	void setTransform(const Eigen::Affine3f& transform) override;

	void resetTransform() override;

	virtual void translate(float x, float y, float z) override;
	virtual void setPosition(float x, float y, float z) override;
	virtual void setScale(float x, float y, float z) override;
	virtual void rotate(float angle, XQ::Vec3f dir) override;

	/**
	 * @brief 旋转变换 angle角度，不是弧度
	 */
	virtual void rotateX(float angle) override;
	virtual void rotateY(float angle) override;
	virtual void rotateZ(float angle) override;
	virtual void scale(float x, float y, float z) override;
protected:
	XRenderNode();
	~XRenderNode();
public:
	int64_t getID() const override;
	bool isSelf(uint64_t id) const{
		return getID() == id;
	}

	void findNodeById(uint64_t id, sptr<XRenderNode>& findNode);
	sptr<XRenderNode> findNodeById(uint64_t id);
public:
	csptr<XDataListT<XBaseRenderNode>> renderNodes;
protected:
	Eigen::Affine3f m_transform = Eigen::Affine3f::Identity();
	uint64_t m_id;
	sptr<xShaderManger> m_shaderManger;
};

class XRENDERNODE_API XDrawableRenderNode :public XRenderNode {
	REGISTER_CLASS_META_DATA(XDrawableRenderNode, XRenderNode);
protected:
	XDrawableRenderNode();
	~XDrawableRenderNode();
public:
	XSIGNAL(void(sptr<XRenderNode>)) sigBeginRender;
	XSIGNAL(void(sptr<XRenderNode>)) sigEndRender;
};

class XRENDERNODE_API XGroupRenderNode :public XRenderNode {
	REGISTER_CLASS_META_DATA(XGroupRenderNode, XRenderNode);
protected:
	XGroupRenderNode();
	~XGroupRenderNode();
};

class XRENDERNODE_API XTransformRenderNode :public XRenderNode {
	REGISTER_CLASS_META_DATA(XTransformRenderNode, XRenderNode);
protected:
	XTransformRenderNode();
	~XTransformRenderNode();
};

class XRENDERNODE_API XRenderNode3D :public XDrawableRenderNode {
	REGISTER_CLASS_META_DATA(XRenderNode3D, XDrawableRenderNode);
protected:
	XRenderNode3D();
	~XRenderNode3D();
};

class XRENDERNODE_API XRenderNode2D :public XDrawableRenderNode {
	REGISTER_CLASS_META_DATA(XRenderNode2D, XDrawableRenderNode);
protected:
	XRenderNode2D();
	~XRenderNode2D();
};