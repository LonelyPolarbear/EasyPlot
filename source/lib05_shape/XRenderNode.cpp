#include "XRenderNode.h"

#include <atomic>
static std::atomic< uint64_t>  object_id_counter(0);

void XRenderNode::Init()
{
	XDataObject::Init();
	XQ_XDATA_ADD(renderNodes);
}

bool XRenderNode::addChild(sptr<XRenderNode> child)
{
	//节点不应该重复
	if(!child)
		return false;
	auto iter =std::find(renderNodes->begin(), renderNodes->end(), child );
	if (iter == renderNodes->end()) {
		renderNodes->append(child);
		child->setRenderNodeParent(asDerived<XRenderNode>());
		return true;
	}
	return false;
}

bool XRenderNode::removeChild(sptr<XRenderNode> child)
{
	auto iter = std::find(renderNodes->begin(), renderNodes->end(), child);
	if (iter != renderNodes->end()) {
		renderNodes->remove(*iter);
		return true;
	}
	return false;
}

void XRenderNode::clearChildren()
{
	renderNodes->clear();
}

int XRenderNode::getChildCount() const
{
	return renderNodes->size();
}

sptr<XRenderNode> XRenderNode::getChild(int index) const
{
	if(index < 0 || index >= renderNodes->size())
		return sptr<XRenderNode>();
	return (*renderNodes)[index]->asDerived<XRenderNode>();
}

sptr<XRenderNode> XRenderNode::getRenderNodeParent() const
{
	auto p=getParent();
	if(p)
		return p->asDerived<XRenderNode>();
	return nullptr;
}

void XRenderNode::setRenderNodeParent(sptr<XRenderNode> parent)
{
	setParent(parent);
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
		if (auto parent = getRenderNodeParent()) {
			return parent->getShaderManger();
		}
		return {};
	}
}

XQ::BoundBox XRenderNode::getBoundBox(const Eigen::Matrix4f& m) const
{
	auto boundBox =getThisBoundBox(m);
	for (auto iter = renderNodes->begin();iter != renderNodes->end();iter++) {
		boundBox.merge((*iter)->asDerived<XRenderNode>()->getThisBoundBox(m * m_transform.matrix()));
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
