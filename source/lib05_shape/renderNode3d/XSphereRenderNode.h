#pragma once
#include <lib05_shape/XGeometryNode.h>
#include <lib05_shape/datasource/xsphereSource.h>
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
	void draw(const Eigen::Matrix4f& parentMatrix, bool isNormal) override;
protected:
	sptr<XSphereSource> m_inputSource;
};