#include "XInfinitePlaneRenderNode.h"
#include <lib04_opengl/XOpenGLEnable.h>
#include <lib01_shader/xshaderManger.h>
#include <lib04_opengl/XOpenGLBuffer.h>
#include <lib04_opengl/XOpenGLFuntion.h>

XInfinitePlaneRenderNode::XInfinitePlaneRenderNode()
{
	mFeedBackBufferVbo = makeShareDbObject<XOpenGLBuffer>();
	mFeedBackBufferVbo->setBufferType(XOpenGLBuffer::VertexBuffer);
	mFeedBackBufferVbo->setUsagePattern(XOpenGLBuffer::DynamicDraw);
}

XInfinitePlaneRenderNode::~XInfinitePlaneRenderNode()
{
}

void XInfinitePlaneRenderNode::createSource()
{
	m_inputSource = makeShareDbObject<XCustomSource>();
	auto coord = m_inputSource->getVertextCoordArray();
	coord->setNumOfTuple(4);
	coord->setTuple(0, -1, -1, -1);
	coord->setTuple(1, 1, -1, -1);
	coord->setTuple(2, 1, 1, -1);
	coord->setTuple(3, -1, 1, -1);
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

void XInfinitePlaneRenderNode::Init()
{
	XGeometryNode::Init();
	//XQ_ATTR_ADD_INIT(AttrIsWorldGrid,true);
	Attribute->AttrIsValidBoundBox->setValue(false);

	//XQ_ATTR_ADD_INIT(AttrGridSpace, 4);
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

void XInfinitePlaneRenderNode::setFrame(const Eigen::Matrix4f& frame)
{
	m_planeFrame = frame;
}

void XInfinitePlaneRenderNode::draw(sptr<XBaseRender> render, const Eigen::Matrix4f& parentMatrix)
{
	auto glEnableObj = makeShareDbObject<XOpenGLEnable>();
	glEnableObj->save();
	glEnableObj->enable(XOpenGLEnable::EnableType::BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	auto shader = getShaderManger()->getGridShader3D();
	shader->setObjectID(getID());
	if (isFeedbackInit == false) {
		shader->addFeedbackShader({ "fragPos3D" });
		isFeedbackInit = true;
		createFeedBack();
	}
	

	shader->use();
	shader->setMat4("gridPlaneMat", m_planeFrame);
	shader->setBool("u_isInfinite", true);
	//shader->setFloat("gridSpace", AttrGridSpace->getValue());
	shader->setFloat("mainGridDensity", AttrMainGridDensity->getValue());
	shader->setFloat("subGridDensity", AttrSubGridDensity->getValue());
	shader->setFloat("mainGridLineWidth", AttrMainGridLineWidth->getValue());
	shader->setFloat("subGridLineWidth", AttrSubGridLineWidth->getValue());
	shader->setFloat("mainGridAlphaFactor", AttrMainGridAlphaFactor->getValue());
	shader->setFloat("subGridAlphaFactor", AttrSubGridAlphaFactor->getValue());
	this->setPolygonMode(PolygonMode::face);

	//需要使用
	mFeedBackBuffer->bind();																											 //------------------------------ 激活
	glBeginTransformFeedback(GL_TRIANGLES);                                                                           //------------------------------ 启动
	glEnableObj->enable(XOpenGLEnable::EnableType::BLEND);
	glEnableObj->enable(XOpenGLEnable::EnableType::DEPTH_CLAMP);
	auto last = XOpenGLFuntion::xglDepthFunc(XOpenGL::DepthOrStencilCompFunType::XGL_LEQUAL);
	this->draw(render,shader,parentMatrix);
	XOpenGLFuntion::xglDepthFunc(last);
	shader->unUse();

	glEnableObj->restore();

	glEndTransformFeedback();

	//mFeedBackBufferVbo->bind();
	//auto datadd = mFeedBackBufferVbo->map2cpu();
	//auto ss = makeShareDbObject<XDataArray1D<XQ::Vec3f>>();
	//ss->setNumOfTuple(6);
	//ss->memCopy(datadd);

	//sigEndRender(this->asDerived<XRenderNode>());
}

void XInfinitePlaneRenderNode::draw(sptr<XBaseRender> render, std::shared_ptr<xshader> s, const Eigen::Matrix4f& parentMatrix)
{
	auto enable = makeShareDbObject<XOpenGLEnable>();

	XGeometryNode::draw(render, s, parentMatrix);
}

void XInfinitePlaneRenderNode::createFeedBack()
{
	mFeedBackBufferVbo->create();
	mFeedBackBufferVbo->bind();
	mFeedBackBufferVbo->allocate(18 * 3 * 4);
	mFeedBackBufferVbo->release();

	mFeedBackBuffer =makeShareDbObject<XOpenGLBuffer>();
	mFeedBackBuffer->setBufferType(XOpenGLBuffer::TransformFeedbackBuffer);
	mFeedBackBuffer->create();
	mFeedBackBuffer->bind();
	
	mFeedBackBuffer->setFeedbackBufferBindIdx(0, mFeedBackBufferVbo);
}

sptr<XOpenGLBuffer> XInfinitePlaneRenderNode::getFeedBackBuffer()
{
	return mFeedBackBufferVbo;
}

Eigen::Matrix4f XInfinitePlaneRenderNode::getFrame() const
{
	return m_planeFrame;
}
