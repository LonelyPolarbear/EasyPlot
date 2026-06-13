#pragma once
#include <xrendernode/XGeometryNode.h>
#include <xrendernode/datasource/xCustomSource.h>
#include <Eigen/Eigen>

/// <summary>
/// 无限平面渲染节点
/// </summary>
class XInfinitePlaneRenderNode : public XGeometryNode {
	REGISTER_CLASS_META_DATA(XInfinitePlaneRenderNode, XGeometryNode);
protected:
	XInfinitePlaneRenderNode();
	virtual ~XInfinitePlaneRenderNode();
protected:
	void createSource();
public:
	void Init() override;
	void setFrame(const Eigen::Matrix4f& frame);
	void draw(sptr<XBaseRender> render, const Eigen::Matrix4f& parentMatrix) override;
	void draw(sptr<XBaseRender> render, std::shared_ptr<xshader>, const Eigen::Matrix4f& parentMatrix) override;
	void createFeedBack();
	sptr<XOpenGLBuffer> getFeedBackBuffer();
	Eigen::Matrix4f getFrame() const;
public:
	//csptr<XAttr_Float> AttrGridSpace;										///< 网格间距
	csptr<XAttr_Float> AttrMainGridDensity;							///< 主网格密度
	csptr<XAttr_Float> AttrSubGridDensity;								///< 次网格密度
	csptr<XAttr_Float> AttrMainGridLineWidth;						///< 主网格宽度
	csptr<XAttr_Float> AttrSubGridLineWidth;							///< 次网格宽度
	csptr<XAttr_Float> AttrSubGridAlphaFactor;						///< 次网格透明度叠加系数
	csptr<XAttr_Float> AttrMainGridAlphaFactor;						///< 主网格透明度叠加系数
protected:
	sptr<XCustomSource> m_inputSource;
	Eigen::Matrix4f m_planeFrame = Eigen::Matrix4f::Identity();					//平面在世界坐标系下的位姿
	bool isFeedbackInit = false;
	sptr<XOpenGLBuffer> mFeedBackBuffer;
	sptr<XOpenGLBuffer> mFeedBackBufferVbo;
};