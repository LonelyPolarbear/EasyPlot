#pragma once
#include <xrendernode/XGeometryNode.h>
#include <xrendernode/datasource/xTorusSource.h>
#include <Eigen/Eigen>

/// <summary>
/// ¾ØÐÎäÖÈ¾½Úµã
/// </summary>
class XTorusRenderNode : public XGeometryNode {
	REGISTER_CLASS_META_DATA(XTorusRenderNode, XGeometryNode);
protected:
	XTorusRenderNode();
	virtual ~XTorusRenderNode();
protected:
	void createSource();
public:
	void Init() override;
	void draw(const Eigen::Matrix4f& parentMatrix, bool isNormal) override;
	void setStartAngle(float angle);
	void setEndAngle(float angle);
	void setMajorRadius(float radius);
	void setMinorRadius(float radius);

	float getStartAngle() const;
	float getEndAngle() const;
	float getMajorRadius() const;
	float getMinorRadius() const;
protected:
	sptr<XTorusSource> m_inputSource;
};