#include "XTorusRenderNode.h"
#include <lib04_opengl/XOpenGLEnable.h>
#include <lib01_shader/xshaderManger.h>
#include <lib04_opengl/XOpenGLBuffer.h>

XTorusRenderNode::XTorusRenderNode()
{

}

XTorusRenderNode::~XTorusRenderNode()
{
}

void XTorusRenderNode::createSource()
{
	m_inputSource = makeShareDbObject<XTorusSource>();
	m_inputSource->Modified();

	this->setInput(m_inputSource);
}

void XTorusRenderNode::Init()
{
	XGeometryNode::Init();

	//!
	//! source´´½¨
	createSource();
}

void XTorusRenderNode::draw(sptr<XBaseRender> render, const Eigen::Matrix4f& parentMatrix, bool isNormal)
{
	XGeometryNode::draw(render,parentMatrix,isNormal);
}

void XTorusRenderNode::setStartAngle(float angle)
{
	m_inputSource->setStartAngle(angle);
}

void XTorusRenderNode::setEndAngle(float angle)
{
	m_inputSource->setEndAngle(angle);
}

void XTorusRenderNode::setMajorRadius(float radius)
{
	m_inputSource->setMajorRadius(radius);
}

void XTorusRenderNode::setMinorRadius(float radius)
{
	m_inputSource->setMinorRadius(radius);
}

float XTorusRenderNode::getStartAngle() const
{
	return m_inputSource->getStartAngle();
}

float XTorusRenderNode::getEndAngle() const
{
	return m_inputSource->getEndAngle();
}

float XTorusRenderNode::getMajorRadius() const
{
	return m_inputSource->getMajorRadius();
}

float XTorusRenderNode::getMinorRadius() const
{
	return m_inputSource->getMinorRadius();
}
