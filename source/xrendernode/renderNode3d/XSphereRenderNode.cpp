#include "XSphereRenderNode.h"
#include <lib04_opengl/XOpenGLEnable.h>
#include <lib01_shader/xshaderManger.h>
#include <lib04_opengl/XOpenGLBuffer.h>

XSphereRenderNode::XSphereRenderNode()
{

}

XSphereRenderNode::~XSphereRenderNode()
{
}

void XSphereRenderNode::createSource()
{
	m_inputSource = makeShareDbObject<XSphereSource>();
	
	m_inputSource->setRadius(1);
	
	this->setInput(m_inputSource);

	m_inputSource->Modified();
}

void XSphereRenderNode::Init()
{
	XGeometryNode::Init();

	//!
	//! source´´½¨
	createSource();
}

void XSphereRenderNode::draw(const Eigen::Matrix4f& parentMatrix, bool isNormal)
{
	XGeometryNode::draw(parentMatrix,isNormal);
}
