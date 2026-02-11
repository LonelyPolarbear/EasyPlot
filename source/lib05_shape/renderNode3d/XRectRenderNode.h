#pragma once
#include <lib05_shape/XGeometryNode.h>
#include <lib05_shape/datasource/xCustomSource.h>
#include <Eigen/Eigen>

/// <summary>
/// ¾ØÐÎäÖÈ¾½Úµã
/// </summary>
class XRectRenderNode : public XGeometryNode {
protected:
	XRectRenderNode();
	virtual ~XRectRenderNode();
protected:
	void createSource();
public:
	void Init() override;
	void setRect(std::vector<XQ::Vec3f> points);
	void draw(const Eigen::Matrix4f& parentMatrix) override;
protected:
	sptr<XCustomSource> m_inputSource;
};