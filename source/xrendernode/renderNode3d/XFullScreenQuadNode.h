#pragma once
#include <xrendernode/XGeometryNode.h>
#include <xrendernode/datasource/xCustomSource.h>
#include <Eigen/Eigen>


class XFullScreenQuadNode : public XGeometryNode {
	REGISTER_CLASS_META_DATA(XFullScreenQuadNode, XGeometryNode);
protected:
	XFullScreenQuadNode();
	virtual ~XFullScreenQuadNode();
protected:
	void createSource();
public:
	void Init() override;
	void setRect(std::vector<XQ::Vec3f> points);
	void setNearRect();
	void setFarRect();
	void draw(const Eigen::Matrix4f& parentMatrix, bool isNormal) override;
	void draw(std::shared_ptr<xshader>, const Eigen::Matrix4f& parentMatrix) override;

	void setVertexColor(std::vector<XQ::XColor> colors);
protected:
	sptr<XCustomSource> m_inputSource;
};