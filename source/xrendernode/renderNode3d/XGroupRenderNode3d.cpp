#include "XGroupRenderNode3d.h"
#include <lib04_opengl/XOpenGLEnable.h>
#include <lib01_shader/xshaderManger.h>
#include <lib04_opengl/XOpenGLBuffer.h>

XGroupRenderNode3d::XGroupRenderNode3d()
{

}

XGroupRenderNode3d::~XGroupRenderNode3d()
{
}

void XGroupRenderNode3d::Init()
{
	XGroupRenderNode::Init();
}

void XGroupRenderNode3d::draw(sptr<XBaseRender> render, const Eigen::Matrix4f& parentMatrix, bool isNormal)
{
	int count = getChildRenderNodeCount();
	for (int i = 0; i < count; i++)
	{
		auto node =getChildRenderNode(i);
		node->draw(render,parentMatrix * m_transform.matrix(),isNormal);
	}
}
