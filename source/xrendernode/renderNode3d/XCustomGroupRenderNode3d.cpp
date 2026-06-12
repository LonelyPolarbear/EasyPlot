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
	XQ_ATTR_ADD_INIT(AttrGridNodeDrawFirst,false);
}

void XCustomGroupRenderNode3d::draw(sptr<XBaseRender> render, const Eigen::Matrix4f& parentMatrix)
{
	int count = getChildRenderNodeCount();
	std::vector<sptr<XBaseRenderNode>> infinitegridNode;
	std::vector<sptr<XBaseRenderNode>> otherNode;
	for (int i = 0; i < count; i++)
	{
		auto node =getChildRenderNode(i);
		if (node->asDerived<XInfinitePlaneRenderNode>()) {
			infinitegridNode.push_back(node);
		}
		else {
			otherNode.push_back(node);
			
		}
	}

	if (AttrGridNodeDrawFirst->getValue()) {
		for (auto n : infinitegridNode) {
			n->draw(render, parentMatrix * m_transform.matrix());
		}

		for (auto n : otherNode) {
			n->draw(render, parentMatrix * m_transform.matrix());
		}
	}
	else {
		for (auto n : otherNode) {
			n->draw(render, parentMatrix * m_transform.matrix());
		}

		for (auto n : infinitegridNode) {
			n->draw(render, parentMatrix * m_transform.matrix());
		}
	}
}
