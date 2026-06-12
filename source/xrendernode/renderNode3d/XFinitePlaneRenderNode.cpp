#include "XFinitePlaneRenderNode.h"
#include <lib04_opengl/XOpenGLEnable.h>
#include <lib01_shader/xshaderManger.h>
#include <lib04_opengl/XOpenGLBuffer.h>
#include <lib04_opengl/XOpenGLFuntion.h>

XFinitePlaneRenderNode::XFinitePlaneRenderNode()
{
	mFeedBackBufferVbo = makeShareDbObject<XOpenGLBuffer>();
	mFeedBackBufferVbo->setBufferType(XOpenGLBuffer::VertexBuffer);
	mFeedBackBufferVbo->setUsagePattern(XOpenGLBuffer::DynamicDraw);
}

XFinitePlaneRenderNode::~XFinitePlaneRenderNode()
{
}

void XFinitePlaneRenderNode::createSource()
{
	m_inputSource = makeShareDbObject<XCustomSource>();
	auto coord = m_inputSource->getVertextCoordArray();
	coord->setNumOfTuple(4);
	coord->setTuple(0, -1, -1, 0);
	coord->setTuple(1, 1, -1, 0);
	coord->setTuple(2, 1, 1, 0);
	coord->setTuple(3, -1, 1, 0);
	coord->Modified();

	auto index = m_inputSource->getFaceIndexArray();
	index->setNumOfTuple(6);
	index->setTuple(0, 0, 1, 2);
	index->setTuple(1, 0, 2, 3);
	index->Modified();

	m_inputSource->Modified();

	this->setColorMode(ColorMode::SingleColor);
	this->setSingleColor(XQ::Vec4f(1, 1, 1, 1));

	this->setInput(m_inputSource);
}

void XFinitePlaneRenderNode::Init()
{
	XGeometryNode::Init();
	//XQ_ATTR_ADD_INIT(AttrIsWorldGrid,true);
	Attribute->AttrIsValidBoundBox->setValue(false);

	XQ_ATTR_ADD_INIT(AttrGridSpace, 4);
	XQ_ATTR_ADD_INIT(AttrMainGridDensity, 20);
	XQ_ATTR_ADD_INIT(AttrSubGridDensity, 4);
	XQ_ATTR_ADD_INIT(AttrMainGridLineWidth, 1);
	XQ_ATTR_ADD_INIT(AttrSubGridLineWidth, 1);
	XQ_ATTR_ADD_INIT(AttrMainGridAlphaFactor, 0.8);
	XQ_ATTR_ADD_INIT(AttrSubGridAlphaFactor, 0.3);
	//!
	//! source创建
	createSource();
}

void XFinitePlaneRenderNode::setFrame(const Eigen::Matrix4f& frame)
{
	m_planeAffineFrame.matrix() = frame;
}

Eigen::Affine3f& XFinitePlaneRenderNode::getGraidAffine()
{
	return m_planeAffineFrame;
}

const Eigen::Affine3f& XFinitePlaneRenderNode::getGraidAffine() const
{
	return m_planeAffineFrame;
}

void XFinitePlaneRenderNode::draw(sptr<XBaseRender> render, const Eigen::Matrix4f& parentMatrix)
{
	auto glEnableObj = makeShareDbObject<XOpenGLEnable>();
	glEnableObj->save();
	glEnableObj->enable(XOpenGLEnable::EnableType::BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	auto shader = getShaderManger()->getGridShader3D();
	shader->setObjectID(getID());

	shader->use();
	shader->setMat4("gridPlaneMat", m_planeAffineFrame.matrix());
	shader->setBool("u_isInfinite", false);
	shader->setFloat("gridSpace", AttrGridSpace->getValue());
	shader->setFloat("mainGridDensity", AttrMainGridDensity->getValue());
	shader->setFloat("subGridDensity", AttrSubGridDensity->getValue());
	shader->setFloat("mainGridLineWidth", AttrMainGridLineWidth->getValue());
	shader->setFloat("subGridLineWidth", AttrSubGridLineWidth->getValue());
	shader->setFloat("mainGridAlphaFactor", AttrMainGridAlphaFactor->getValue());
	shader->setFloat("subGridAlphaFactor", AttrSubGridAlphaFactor->getValue());
	this->setPolygonMode(PolygonMode::face);

	//需要使用
	glEnableObj->enable(XOpenGLEnable::EnableType::BLEND);
	glEnableObj->enable(XOpenGLEnable::EnableType::DEPTH_CLAMP);
	auto last = XOpenGLFuntion::xglDepthFunc(XOpenGL::DepthOrStencilCompFunType::XGL_LEQUAL);
	this->draw(render,shader,parentMatrix);
	XOpenGLFuntion::xglDepthFunc(last);
	shader->unUse();

	glEnableObj->restore();
}

void XFinitePlaneRenderNode::draw(sptr<XBaseRender> render, std::shared_ptr<xshader> s, const Eigen::Matrix4f& parentMatrix)
{
	auto enable = makeShareDbObject<XOpenGLEnable>();

	XGeometryNode::draw(render, s, parentMatrix);
}


//void XInfinitePlaneRenderNode::setRect(std::vector<XQ::Vec3f> points)
//{
//	auto coord = m_inputSource->getVertextCoordArray();
//	coord->setTuple(0, points[0].x(), points[0].y(), points[0].z());
//	coord->setTuple(1, points[1].x(), points[1].y(), points[1].z());
//	coord->setTuple(2, points[2].x(), points[2].y(), points[2].z());
//	coord->setTuple(3, points[3].x(), points[3].y(), points[3].z());
//	coord->Modified();
//	m_inputSource->Modified();
//}


Eigen::Matrix4f XFinitePlaneRenderNode::getFrame() const
{
	return m_planeAffineFrame.matrix();
}
