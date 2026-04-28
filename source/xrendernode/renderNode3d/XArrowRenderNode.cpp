#include "XArrowRenderNode.h"
#include <lib04_opengl/XOpenGLEnable.h>
#include <lib01_shader/xshaderManger.h>
#include <lib04_opengl/XOpenGLBuffer.h>
#include <xrendernode/datasource/xCustomSource.h>
#include <xrendernode/datasource/xconeSource.h>
#include <xrendernode/datasource/xcylinderSource.h>
#include <xrendernode/filter/xshapeSourceCombineFilter.h>
#include <xrendernode/filter/xshapeSourceTransformFilter.h>
#include <Eigen/Eigen>

class XArrowRenderNode::Internal {
public:
	sptr<XShapeSourceCombineFilter> m_inputSource;
	sptr< XShapeSourceTransformFilter> m_coneTransformSource;
	sptr< XShapeSourceTransformFilter> m_cylinderTransformSource;
	sptr<XConeSource> m_coneSource;
	sptr<XCylinderSource> m_cylinderSource;

	void createSource() {
		m_coneSource = makeShareDbObject<XConeSource>();
		m_cylinderSource = makeShareDbObject<XCylinderSource>();

		m_coneTransformSource = makeShareDbObject<XShapeSourceTransformFilter>();
		m_coneTransformSource->setInput(m_coneSource);

		m_cylinderTransformSource = makeShareDbObject<XShapeSourceTransformFilter>();
		m_cylinderTransformSource->setInput(m_cylinderSource);

		m_inputSource = makeShareDbObject<XShapeSourceCombineFilter>();
		m_inputSource->addInput(m_cylinderTransformSource);
		m_inputSource->addInput(m_coneTransformSource);
	}
};

XArrowRenderNode::XArrowRenderNode():mData(new Internal)
{

}

XArrowRenderNode::~XArrowRenderNode()
{
}

void XArrowRenderNode::createSource()
{
	mData->createSource();
	this->setInput(mData->m_inputSource);
	setLineSize(1,1);
	setArrowSize(1,1);
}

void XArrowRenderNode::Init()
{
	XGeometryNode::Init();

	//!
	//! source创建
	createSource();
}

void XArrowRenderNode::setArrowSize(double r, double h)
{
	//mData->m_coneTransformSource->setScale(r, r, h);

	//线的缩放，表示线的长度，圆柱的默认高度是1，默认上下底面分别在Z=±0.5的位置
	auto lineScale =mData->m_cylinderTransformSource->getScale();
	Eigen::Affine3f arrowTrans = Eigen::Affine3f::Identity();
	arrowTrans.translate(Eigen::Vector3f( 0, 0, lineScale[2]));
	arrowTrans.scale(Eigen::Vector3f(r, r, h));

	arrowTrans.translate(Eigen::Vector3f(0,0,0.5));			//调整初始位置

	mData->m_coneTransformSource->setTransform(arrowTrans);
}

void XArrowRenderNode::setLineSize(double r, double h)
{
	Eigen::Affine3f lineTrans = Eigen::Affine3f::Identity();
	
	lineTrans.scale(Eigen::Vector3f(r, r, h));
	lineTrans.translate(Eigen::Vector3f(0, 0, 0.5));			//调整初始位置

	mData->m_cylinderTransformSource->setTransform(lineTrans);

	auto coneScale = mData->m_coneTransformSource->getScale();

	setArrowSize(coneScale[0], coneScale[2]);
}

void XArrowRenderNode::draw(const Eigen::Matrix4f& parentMatrix, bool isNormal)
{
	XGeometryNode::draw(parentMatrix,isNormal);
}
