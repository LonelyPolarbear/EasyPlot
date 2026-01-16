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
protected:
	sptr<XCustomSource> m_inputSource;
	Eigen::Matrix4f m_planeFrame;					//平面在世界坐标系下的位姿
};