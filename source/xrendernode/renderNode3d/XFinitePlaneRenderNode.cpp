#include "XFinitePlaneRenderNode.h"
#include <lib04_opengl/XOpenGLEnable.h>
#include <lib01_shader/xshaderManger.h>
#include <lib04_opengl/XOpenGLBuffer.h>
#include <lib04_opengl/XOpenGLFuntion.h>
#include <xalgo/XAlgo.h>
#include <xlog/XLogger.h>

XFinitePlaneRenderNode::XFinitePlaneRenderNode()
{
	mFeedBackBufferVbo = makeShareDbObject<XOpenGLBuffer>();
	mFeedBackBufferVbo->setBufferType(XOpenGLBuffer::VertexBuffer);
	mFeedBackBufferVbo->setUsagePattern(XOpenGLBuffer::DynamicDraw);
}

XFinitePlaneRenderNode::~XFinitePlaneRenderNode()
{
	mConnector.disconnect();
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

	XQ_ATTR_ADD_INIT(AttrGridSpacex, 4);
	XQ_ATTR_ADD_INIT(AttrGridSpacey, 4);
	XQ_ATTR_ADD_INIT(AttrMainGridNum, 5);
	XQ_ATTR_ADD_INIT(AttrSubGridNum, 4);
	XQ_ATTR_ADD_INIT(AttrMainGridLineWidth, 1);
	XQ_ATTR_ADD_INIT(AttrSubGridLineWidth, 1);
	XQ_ATTR_ADD_INIT(AttrMainGridAlphaFactor, 0.8);
	XQ_ATTR_ADD_INIT(AttrSubGridAlphaFactor, 0.3);

	XQ_ATTR_ADD_INIT(AttrXRange, XQ::Vec2f(-1, 1));
	XQ_ATTR_ADD_INIT(AttrYRange, XQ::Vec2f(-1, 1));
	//!
	//! source创建
	createSource();

	mConnector.connect(AttrXRange,&XAttr_Vec2f::sigAttrChanged,[this](sptr<XDataAttribute>, XDataChangeType type){
		if (type == XDataChangeType::ItemDataModified) {
			slotUpdateGridAffine();
		}
	});

	mConnector.connect(AttrYRange, &XAttr_Vec2f::sigAttrChanged, [this](sptr<XDataAttribute>, XDataChangeType type) {
		if (type == XDataChangeType::ItemDataModified) {
			slotUpdateGridAffine();
		}
		});
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

	auto shader = getShaderManger()->getGridShader();
	shader->setObjectID(getID());

	shader->use();
	shader->setMat4("gridPlaneMat", m_planeAffineFrame.matrix());
	shader->setBool("u_isInfinite", false);
	shader->setFloat("gridSpacex", AttrGridSpacex->getValue());
	shader->setFloat("gridSpacey", AttrGridSpacey->getValue());
	shader->setInt("mainGridNum", AttrMainGridNum->getValue());
	shader->setInt("subGridNum", AttrSubGridNum->getValue());
	shader->setFloat("mainGridLineWidth", AttrMainGridLineWidth->getValue());
	shader->setFloat("subGridLineWidth", AttrSubGridLineWidth->getValue());
	shader->setFloat("mainGridAlphaFactor", AttrMainGridAlphaFactor->getValue());
	shader->setFloat("subGridAlphaFactor", AttrSubGridAlphaFactor->getValue());
	shader->setFloat("isShowGrid", true);
	this->setPolygonMode(PolygonMode::face);

	//需要使用
	glEnableObj->enable(XOpenGLEnable::EnableType::BLEND);
	//glEnableObj->disable(XOpenGLEnable::EnableType::DEPTH_CLAMP);
	auto last = XOpenGLFuntion::xglDepthFunc(XOpenGL::DepthOrStencilCompFunType::XGL_LEQUAL);
	this->draw(render,shader,parentMatrix);
	XOpenGLFuntion::xglDepthFunc(last);
	shader->unUse();

	glEnableObj->restore();
}

void XFinitePlaneRenderNode::draw(sptr<XBaseRender> render, std::shared_ptr<xshader> s, const Eigen::Matrix4f& parentMatrix)
{
	auto enable = makeShareDbObject<XOpenGLEnable>();

	auto old = XOpenGLFuntion::xglDrawBuffers({XOpenGL::XGL_COLOR_ATTACHMENT0});
	
	XGeometryNode::draw(render, s, parentMatrix);

	XOpenGLFuntion::xglDrawBuffers(old);
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

void XFinitePlaneRenderNode::setXRange(float min, float max)
{
	AttrXRange->setValue(XQ::Vec2f(min,max));
}

void XFinitePlaneRenderNode::setYRange(float min, float max)
{
	AttrYRange->setValue(XQ::Vec2f(min, max));
}

void XFinitePlaneRenderNode::gridTranslate(float tx, float ty)
{
	auto rangex = AttrXRange->getValue();
	rangex[0]+=tx;
	rangex[1]+=tx;
	AttrXRange->setValue(rangex);

	auto rangey = AttrYRange->getValue();
	rangey[0] += ty;
	rangey[1] += ty;
	AttrYRange->setValue(rangey);
}

void XFinitePlaneRenderNode::gridScale(float sx, float sy, const XQ::Vec2f center)
{
	auto rangex = AttrXRange->getValue();
	auto rangey = AttrYRange->getValue();

	auto fx2 =1-XQ::XAlgo::getInterpolationCoefficient(rangex[0],rangex[1],center[0]);
	auto len_x = rangex[1]-rangex[0];
	len_x *=sx;

	rangex[1] = center[0]+len_x*fx2;
	rangex[0] = rangex[1]-len_x;

	auto fy2 = 1 - XQ::XAlgo::getInterpolationCoefficient(rangey[0], rangey[1], center[1]);
	auto len_y = rangey[1]-rangey[0];
	len_y*=sy;

	rangey[1] = center[1] + len_y * fy2;
	rangey[0] = rangey[1] - len_y;

	setXRange(rangex[0], rangex[1]);
	setYRange(rangey[0], rangey[1]);
}

void XFinitePlaneRenderNode::slotUpdateGridAffine()
{
	auto xrange = AttrXRange->getValue();
	auto yrange = AttrYRange->getValue();
	auto scale_x = (xrange[1] - xrange[0]) * 0.5;
	auto scale_y = (yrange[1] - yrange[0]) * 0.5;
	m_planeAffineFrame.setIdentity();
	m_planeAffineFrame.scale(Eigen::Vector3f(1.f/scale_x, 1.f /scale_y, 1));

	auto tx = -1*scale_x - xrange[0];
	auto ty = -1*scale_y - yrange[0];

	m_planeAffineFrame.translate(Eigen::Vector3f(tx, ty, 0));
	//XLOG_INFO("**********************{},{}",tx,ty);
}
