#pragma once
#include <xrendernode/renderNode3d/XFullScreenQuadNode.h>
#include <Eigen/Eigen>


class XSmaaFullScreenQuadNode : public XFullScreenQuadNode {
	REGISTER_CLASS_META_DATA(XSmaaFullScreenQuadNode, XFullScreenQuadNode);
protected:
	XSmaaFullScreenQuadNode();
	virtual ~XSmaaFullScreenQuadNode();
public:
	void draw(sptr<XBaseRender> render, const Eigen::Matrix4f& parentMatrix) override;
	void draw(sptr<XBaseRender> render, std::shared_ptr<xshader>, const Eigen::Matrix4f& parentMatrix) override;
	void Init() override;
	void setInputColorTexture(sptr<XDataBaseObject> colorTexture, sptr<XDataBaseObject> depthTexture);
protected:
	void drawSmaa(sptr<XBaseRender> render, const Eigen::Matrix4f& parentMatrix);
	void drawNoSmaa(sptr<XBaseRender> render, const Eigen::Matrix4f& parentMatrix);
public:
	csptr<XAttr_Bool> AttrEnableSmaa;
protected:
	class Internal;
	std::unique_ptr<Internal> mData;
};