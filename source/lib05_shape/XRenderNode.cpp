#include "XRenderNode.h"

#include <atomic>
static std::atomic< uint64_t>  object_id_counter(0);

bool XRenderNode::addChild(sptr<XRenderNode> child)
{
	//节点不应该重复
	if(!child)
		return false;
	auto iter =std::find(m_children.begin(), m_children.end(), child );
	if (iter == m_children.end()) {
		m_children.push_back(child);
		child->setParent(asDerived<XRenderNode>());
		return true;
	}
	return false;
}

bool XRenderNode::removeChild(sptr<XRenderNode> child)
{
	auto iter = std::find(m_children.begin(), m_children.end(), child);
	if (iter != m_children.end()) {
		m_children.erase(iter);
		return true;
	}
	return false;
}

void XRenderNode::clearChildren()
{
	m_children.clear();
}

int XRenderNode::getChildCount() const
{
	return m_children.size();
}

sptr<XRenderNode> XRenderNode::getChild(int index) const
{
	if(index < 0 || index >= m_children.size())
		return sptr<XRenderNode>();
	return m_children[index];
}

sptr<XRenderNode> XRenderNode::getParent() const
{
	return m_parent.lock();
}

void XRenderNode::setParent(sptr<XRenderNode> parent)
{
	m_parent = parent;
}

void XRenderNode::setShaderManger(std::shared_ptr<xShaderManger> shaderManger)
{
	m_shaderManger = shaderManger;
}

std::shared_ptr<xShaderManger> XRenderNode::getShaderManger() const
{
	if (m_shaderManger) {
		return m_shaderManger;
	}
	else {
		if (auto parent = getParent()) {
			return parent->getShaderManger();
		}
		return {};
	}
}

XQ::BoundBox XRenderNode::getBoundBox(const Eigen::Matrix4f& m) const
{
	auto boundBox =getThisBoundBox(m);
	for (auto c : m_children) {
		boundBox.merge(c->getThisBoundBox(m * m_transform.matrix()));
	}
	return boundBox;
}

XRenderNode::XRenderNode()
{
	m_id = ++object_id_counter;
}

XRenderNode::~XRenderNode()
{
}

int64_t XRenderNode::getID() const
{
	return m_id;
}

//--------------------------------------------------------------
XDrawableRenderNode::XDrawableRenderNode()
{
}

XDrawableRenderNode::~XDrawableRenderNode()
{
}

//--------------------------------------------------------------
XRenderNode3D::XRenderNode3D()
{
}

XRenderNode3D::~XRenderNode3D()
{
}

//--------------------------------------------------------------
XRenderNode2D::XRenderNode2D()
{
}

XRenderNode2D::~XRenderNode2D()
{
}

XGroupRenderNode::XGroupRenderNode()
{
}

XGroupRenderNode::~XGroupRenderNode()
{
}

XTransformRenderNode::XTransformRenderNode()
{
}

XTransformRenderNode::~XTransformRenderNode()
{
}
