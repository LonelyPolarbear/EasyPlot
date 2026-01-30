#pragma once
#include <lib05_shape/XGeometryNode.h>
#include <lib05_shape/datasource/xCustomSource.h>
#include <Eigen/Eigen>

/// <summary>
/// 无限平面渲染节点
/// </summary>
class XInfinitePlaneRenderNode : public XGeometryNode {
protected:
	XInfinitePlaneRenderNode();
	virtual ~XInfinitePlaneRenderNode();
protected:
	void createSource();
public:
	void Init() override;
	void setFrame(const Eigen::Matrix4f& frame);
	void draw() override;
	void draw(std::shared_ptr<xshader>) override;
	void createFeedBack();
	void setRect(std::vector<XQ::Vec3f> points);
	sptr<XOpenGLBuffer> getFeedBackBuffer();
protected:
	sptr<XCustomSource> m_inputSource;
	Eigen::Matrix4f m_planeFrame = Eigen::Matrix4f::Identity();					//平面在世界坐标系下的位姿
	bool isFeedbackInit = false;
	sptr<XOpenGLBuffer> mFeedBackBuffer;
	sptr<XOpenGLBuffer> mFeedBackBufferVbo;
};