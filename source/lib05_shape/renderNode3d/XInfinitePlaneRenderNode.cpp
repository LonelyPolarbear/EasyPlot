#include "XInfinitePlaneRenderNode.h"
#include <lib04_opengl/XOpenGLEnable.h>
#include <lib01_shader/xshaderManger.h>
#include <lib04_opengl/XOpenGLBuffer.h>

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

	//!
	//! source创建
	createSource();
}

void XInfinitePlaneRenderNode::setFrame(const Eigen::Matrix4f& frame)
{
	m_planeFrame = frame;
}

void XInfinitePlaneRenderNode::draw(const Eigen::Matrix4f& parentMatrix,  bool isNormal)
{
	//sigBeginRender(this->asDerived<XRenderNode>());
	if (isNormal == false) {
		//拾取操作
		return;
	}
	auto glEnableObj = makeShareDbObject<XOpenGLEnable>();
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
	Eigen::Affine3f t = Eigen::Affine3f::Identity();
	//t.rotate(Eigen::AngleAxisf(XQ::Matrix::radian(90), Eigen::Vector3f::UnitX()));
	Eigen::Matrix4f mat = t.matrix();
	shader->setMat4("gridPlaneMatInWorld", mat);
	this->setPolygonMode(PolygonMode::face);

	//需要使用
	mFeedBackBuffer->bind();																											 //------------------------------ 激活
	glBeginTransformFeedback(GL_TRIANGLES);                                                                           //------------------------------ 启动

	this->draw(shader,parentMatrix);
	shader->unUse();

	glEnableObj->restore();

	glEndTransformFeedback();

	//mFeedBackBufferVbo->bind();
	//auto datadd = mFeedBackBufferVbo->map2cpu();
	//auto ss = makeShareDbObject<XDataArray<XQ::Vec3f>>();
	//ss->setNumOfTuple(6);
	//ss->memCopy(datadd);

	//sigEndRender(this->asDerived<XRenderNode>());
}

void XInfinitePlaneRenderNode::draw(std::shared_ptr<xshader> s, const Eigen::Matrix4f& parentMatrix)
{
	return XGeometryNode::draw(s,parentMatrix);
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

void XInfinitePlaneRenderNode::setRect(std::vector<XQ::Vec3f> points)
{
	auto coord = m_inputSource->getVertextCoordArray();
	coord->setTuple(0, points[0].x(), points[0].y(), points[0].z());
	coord->setTuple(1, points[1].x(), points[1].y(), points[1].z());
	coord->setTuple(2, points[2].x(), points[2].y(), points[2].z());
	coord->setTuple(3, points[3].x(), points[3].y(), points[3].z());
	coord->Modified();
	m_inputSource->Modified();
}

sptr<XOpenGLBuffer> XInfinitePlaneRenderNode::getFeedBackBuffer()
{
	return mFeedBackBufferVbo;
}
