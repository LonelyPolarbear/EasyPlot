#include "XRectRenderNode.h"
#include <lib04_opengl/XOpenGLEnable.h>
#include <lib01_shader/xshaderManger.h>
#include <lib04_opengl/XOpenGLBuffer.h>

XRectRenderNode::XRectRenderNode()
{

}

XRectRenderNode::~XRectRenderNode()
{
}

void XRectRenderNode::createSource()
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

void XRectRenderNode::Init()
{
	XGeometryNode::Init();

	//!
	//! source´´½¨
	createSource();
}

void XRectRenderNode::setRect(std::vector<XQ::Vec3f> points)
{
	auto coord = m_inputSource->getVertextCoordArray();
	coord->setTuple(0, points[0].x(), points[0].y(), points[0].z());
	coord->setTuple(1, points[1].x(), points[1].y(), points[1].z());
	coord->setTuple(2, points[2].x(), points[2].y(), points[2].z());
	coord->setTuple(3, points[3].x(), points[3].y(), points[3].z());
	coord->Modified();
	m_inputSource->Modified();
}

void XRectRenderNode::draw(sptr<XBaseRender> render, const Eigen::Matrix4f& parentMatrix, bool isNormal)
{
	XGeometryNode::draw(render,parentMatrix,isNormal);
}

void XRectRenderNode::draw(sptr<XBaseRender> render, std::shared_ptr<xshader> s, const Eigen::Matrix4f& parentMatrix)
{
	XGeometryNode::draw(render,s, parentMatrix);
}
