#pragma once
#include "xrendernodeApi.h"
#include <dataBase/XDataBaseObject.h>
#include <dataBase/XDataObject.h>
#include <dataBase/XDataList.h>
#include <xsignal/XSignal.h>
#include <lib00_utilty/XUtilty.h>

#include "XRenderNodeAttribute.h"
#include <Eigen/Eigen>

class xShaderManger;
class xrendernode_API XRenderNode :public XDataObject {
	REGISTER_CLASS_META_DATA(XRenderNode, XDataObject);
public:
	void Init() override;
	/**
	 * @breif 渲染节点的绘制接口
	 * @param parentMatrix 父节点的变换矩阵
	 * @param isNormal true表示正常绘制，false表示拾取
	 */
	virtual void draw(const Eigen::Matrix4f& parentMatrix,bool isNormal) = 0;
	virtual void drawInstance(const Eigen::Matrix4f& parentMatrix)=0;

	bool addChild(sptr<XRenderNode> child);
	bool removeChild(sptr< XRenderNode> child);
	void clearChildren();
	int getChildCount() const;
	sptr<XRenderNode> getChild(int index) const;
	sptr<XRenderNode> getRenderNodeParent() const;

	/**
	 * @brief 获取当前节点的父节点链
	 */
	std::vector< sptr<const XRenderNode>> getRenderNodeChain() const;
	
	virtual void setVisible(bool visible);

	virtual bool isVisible();

	void setShaderManger(std::shared_ptr<xShaderManger> shaderManger);

	std::shared_ptr<xShaderManger> getShaderManger() const;

	//公共的虚口
	virtual XQ::BoundBox getBoundBox(const Eigen::Matrix4f& m) const;

	virtual XQ::BoundBox getThisBoundBox(const Eigen::Matrix4f& m) const { return {}; };

	Eigen::Affine3f getTransform() const;

	void getChainTransform(Eigen::Affine3f& afiine) const;

	void setTransform(const Eigen::Affine3f& transform);

	void resetTransform();

	virtual void translate(float x, float y, float z);
	virtual void setPosition(float x, float y, float z);
	virtual void setScale(float x, float y, float z);
	virtual void rotate(float angle, XQ::Vec3f dir);

	/**
	 * @brief 旋转变换 angle角度，不是弧度
	 */
	virtual void rotateX(float angle);
	virtual void rotateY(float angle);
	virtual void rotateZ(float angle);
	virtual void scale(float x, float y, float z);
protected:
	XRenderNode();
	~XRenderNode();
public:
	int64_t getID() const;
	bool isSelf(uint64_t id) const{
		return getID() == id;
	}

	void findNodeById(uint64_t id, sptr<XRenderNode>& findNode);
	sptr<XRenderNode> findNodeById(uint64_t id);
public:
	csptr<XDataListT<XRenderNode>> renderNodes;
protected:
	Eigen::Affine3f m_transform = Eigen::Affine3f::Identity();
	uint64_t m_id;
	sptr<xShaderManger> m_shaderManger;
};

class xrendernode_API XDrawableRenderNode :public XRenderNode {
	REGISTER_CLASS_META_DATA(XDrawableRenderNode, XRenderNode);
protected:
	XDrawableRenderNode();
	~XDrawableRenderNode();
public:
	XSIGNAL(void(sptr<XRenderNode>)) sigBeginRender;
	XSIGNAL(void(sptr<XRenderNode>)) sigEndRender;
};

class xrendernode_API XGroupRenderNode :public XRenderNode {
	REGISTER_CLASS_META_DATA(XGroupRenderNode, XRenderNode);
protected:
	XGroupRenderNode();
	~XGroupRenderNode();
};

class xrendernode_API XTransformRenderNode :public XRenderNode {
	REGISTER_CLASS_META_DATA(XTransformRenderNode, XRenderNode);
protected:
	XTransformRenderNode();
	~XTransformRenderNode();
};

class xrendernode_API XRenderNode3D :public XDrawableRenderNode {
	REGISTER_CLASS_META_DATA(XRenderNode3D, XDrawableRenderNode);
protected:
	XRenderNode3D();
	~XRenderNode3D();
};

class xrendernode_API XRenderNode2D :public XDrawableRenderNode {
	REGISTER_CLASS_META_DATA(XRenderNode2D, XDrawableRenderNode);
protected:
	XRenderNode2D();
	~XRenderNode2D();
};