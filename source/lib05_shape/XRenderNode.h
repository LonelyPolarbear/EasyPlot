#pragma once
#include "lib05_shapeApi.h"
#include <dataBase/XDataBaseObject.h>
#include <dataBase/XDataObject.h>
#include <dataBase/XDataList.h>
#include <xsignal/XSignal.h>
#include <lib00_utilty/XUtilty.h>

#include "XRenderNodeAttribute.h"
#include <Eigen/Eigen>

class xShaderManger;
class XRenderNodeAttribute;
class LIB05_SHAPE_API XRenderNode :public XDataObject {
	REGISTER_CLASS_META_DATA(XRenderNode, XDataObject);
public:
	void Init() override;
	virtual void draw(const Eigen::Matrix4f& parentMatrix,bool isNormal/*true表示正常绘制，false表示拾取*/) = 0;
	virtual void drawInstance(const Eigen::Matrix4f& parentMatrix)=0;

	bool addChild(sptr<XRenderNode> child);
	bool removeChild(sptr<XRenderNode> child);
	void clearChildren();
	int getChildCount() const;
	sptr<XRenderNode> getChild(int index) const;
	sptr<XRenderNode> getRenderNodeParent() const;
	void setRenderNodeParent(sptr<XRenderNode> parent);

	void setShaderManger(std::shared_ptr<xShaderManger> shaderManger);

	std::shared_ptr<xShaderManger> getShaderManger() const;

	//公共的虚口
	XQ::BoundBox getBoundBox(const Eigen::Matrix4f& m) const;
	virtual XQ::BoundBox getThisBoundBox(const Eigen::Matrix4f& m) const { return {}; };
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
	//wptr<XRenderNode> m_parent;													//父节点
	sptr<xShaderManger> m_shaderManger;
};

class LIB05_SHAPE_API XDrawableRenderNode :public XRenderNode {
	REGISTER_CLASS_META_DATA(XDrawableRenderNode, XRenderNode);
protected:
	XDrawableRenderNode();
	~XDrawableRenderNode();
public:
	XSIGNAL(void(sptr<XRenderNode>)) sigBeginRender;
	XSIGNAL(void(sptr<XRenderNode>)) sigEndRender;
};

class LIB05_SHAPE_API XGroupRenderNode :public XRenderNode {
	REGISTER_CLASS_META_DATA(XGroupRenderNode, XRenderNode);
protected:
	XGroupRenderNode();
	~XGroupRenderNode();
};

class LIB05_SHAPE_API XTransformRenderNode :public XRenderNode {
	REGISTER_CLASS_META_DATA(XTransformRenderNode, XRenderNode);
protected:
	XTransformRenderNode();
	~XTransformRenderNode();
};

class LIB05_SHAPE_API XRenderNode3D :public XDrawableRenderNode {
	REGISTER_CLASS_META_DATA(XRenderNode3D, XDrawableRenderNode);
protected:
	XRenderNode3D();
	~XRenderNode3D();
};

class LIB05_SHAPE_API XRenderNode2D :public XDrawableRenderNode {
	REGISTER_CLASS_META_DATA(XRenderNode2D, XDrawableRenderNode);
protected:
	XRenderNode2D();
	~XRenderNode2D();
};