#pragma once
#include <xrendernode/XGeometryNode.h>
#include <xrendernode/datasource/xsphereSource.h>
#include <Eigen/Eigen>

/// <summary>
/// Çò½Úµã
/// </summary>
class XSphereRenderNode : public XGeometryNode {
	REGISTER_CLASS_META_DATA(XSphereRenderNode, XGeometryNode);
protected:
	XSphereRenderNode();
	virtual ~XSphereRenderNode();
protected:
	void createSource();
public:
	void Init() override;
	void draw(sptr<XBaseRender> render, const Eigen::Matrix4f& parentMatrix) override;

protected:
	sptr<XSphereSource> m_inputSource;
};