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

void XGroupRenderNode3d::draw(const Eigen::Matrix4f& parentMatrix, bool isNormal)
{
	int count = getChildCount();
	for (int i = 0; i < count; i++)
	{
		auto node =getChild(i);
		node->draw(parentMatrix,isNormal);
	}
}
