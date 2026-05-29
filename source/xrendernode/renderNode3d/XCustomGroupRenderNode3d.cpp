#include "XCustomGroupRenderNode3d.h"
#include <xrendernode/renderNode3d/XInfinitePlaneRenderNode.h>
#include <lib04_opengl/XOpenGLEnable.h>
#include <lib01_shader/xshaderManger.h>
#include <lib04_opengl/XOpenGLBuffer.h>

XCustomGroupRenderNode3d::XCustomGroupRenderNode3d()
{

}

XCustomGroupRenderNode3d::~XCustomGroupRenderNode3d()
{
}

void XCustomGroupRenderNode3d::Init()
{
	XGroupRenderNode::Init();
}

void XCustomGroupRenderNode3d::draw(sptr<XBaseRender> render, const Eigen::Matrix4f& parentMatrix)
{
	int count = getChildRenderNodeCount();
	std::vector<sptr<XBaseRenderNode>> infinitegridNode;
	for (int i = 0; i < count; i++)
	{
		auto node =getChildRenderNode(i);
		if (node->asDerived<XInfinitePlaneRenderNode>()) {
			infinitegridNode.push_back(node);
		}
		else {
			node->draw(render, parentMatrix * m_transform.matrix());
		}
	}

	//网格节点最后绘制
	for (auto n : infinitegridNode) {
		n->draw(render, parentMatrix * m_transform.matrix());
	}
}
