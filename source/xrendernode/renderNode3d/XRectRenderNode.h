#pragma once
#include <xrendernode/XGeometryNode.h>
#include <xrendernode/datasource/xCustomSource.h>
#include <Eigen/Eigen>

/// <summary>
/// ¾ØÐÎäÖÈ¾½Úµã
/// </summary>
class XRectRenderNode : public XGeometryNode {
	REGISTER_CLASS_META_DATA(XRectRenderNode, XGeometryNode);
protected:
	XRectRenderNode();
	virtual ~XRectRenderNode();
protected:
	void createSource();
public:
	void Init() override;
	void setRect(std::vector<XQ::Vec3f> points);
	void draw(sptr<XBaseRender> render, const Eigen::Matrix4f& parentMatrix, bool isNormal) override;
	void draw(sptr<XBaseRender> render, std::shared_ptr<xshader>, const Eigen::Matrix4f& parentMatrix) override;
protected:
	sptr<XCustomSource> m_inputSource;
};