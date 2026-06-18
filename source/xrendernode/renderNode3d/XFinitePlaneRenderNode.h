#pragma once
#include <xrendernode/XGeometryNode.h>
#include <xrendernode/datasource/xCustomSource.h>
#include <Eigen/Eigen>
#include <xsignal/XSignal.h>

/// <summary>
/// 无限平面渲染节点
/// </summary>
class XFinitePlaneRenderNode : public XGeometryNode {
	REGISTER_CLASS_META_DATA(XFinitePlaneRenderNode, XGeometryNode);
protected:
	XFinitePlaneRenderNode();
	virtual ~XFinitePlaneRenderNode();
protected:
	void createSource();
public:
	void Init() override;
	void setFrame(const Eigen::Matrix4f& frame);
	Eigen::Affine3f& getGraidAffine();
	const Eigen::Affine3f& getGraidAffine() const;
	void draw(sptr<XBaseRender> render, const Eigen::Matrix4f& parentMatrix) override;
	void draw(sptr<XBaseRender> render, std::shared_ptr<xshader>, const Eigen::Matrix4f& parentMatrix) override;
	Eigen::Matrix4f getFrame() const;

	void setXRange(float min, float max);
	void setYRange(float min, float max);

	void gridTranslate(float tx,float ty);
	void gridScale(float sx,float sy,const XQ::Vec2f center);
protected:
	void slotUpdateGridAffine();
public:
	csptr<XAttr_Float> AttrGridSpacex;											///< 网格X间距
	csptr<XAttr_Float> AttrGridSpacey;											///< 网格Y间距
	csptr<XAttr_Float> AttrMainGridNum;										///< 主网格密度
	csptr<XAttr_Float> AttrSubGridNum;										///< 次网格密度
	csptr<XAttr_Float> AttrMainGridLineWidth;							///< 主网格宽度
	csptr<XAttr_Float> AttrSubGridLineWidth;								///< 次网格宽度
	csptr<XAttr_Float> AttrSubGridAlphaFactor;							///< 次网格透明度叠加系数
	csptr<XAttr_Float> AttrMainGridAlphaFactor;							///< 主网格透明度叠加系数

	csptr<XAttr_Vec2f> AttrXRange;
	csptr<XAttr_Vec2f> AttrYRange;
protected:
	sptr<XCustomSource> m_inputSource;
	Eigen::Affine3f m_planeAffineFrame = Eigen::Affine3f::Identity();
	bool isFeedbackInit = false;
	sptr<XOpenGLBuffer> mFeedBackBuffer;
	sptr<XOpenGLBuffer> mFeedBackBufferVbo;

	xsig::xconnector mConnector;
};