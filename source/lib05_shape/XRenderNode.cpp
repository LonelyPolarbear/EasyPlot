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
		//child->setRenderNodeParent(asDerived<XRenderNode>()->renderNodes);
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
	if (p) {
		if (p->getClassName() ==XQ_META::ClassName<XDataListT<XRenderNode>>()) {
			p = p->getParent();
		}
		
		if (p) {
			return p->asDerived<XRenderNode>();
		}
	}
		
	return nullptr;
}

void XRenderNode::setVisible(bool visible)
{
	AttrVisible->setValue(visible);
}

bool XRenderNode::isVisible()
{
	auto parent = getRenderNodeParent();
	if (!parent) {
		return AttrVisible->getValue();
	}
	else {
		return AttrVisible->getValue() && parent->isVisible();
	}
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
		boundBox.merge((*iter)->asDerived<XRenderNode>()->getBoundBox(m * m_transform.matrix()));
	}
	return boundBox;
}

Eigen::Affine3f XRenderNode::getTransform() const
{
	return m_transform;
}

void XRenderNode::setTransform(const Eigen::Affine3f& transform)
{
	m_transform = transform;
}

void XRenderNode::resetTransform()
{
	m_transform = Eigen::Affine3f::Identity();
}

void XRenderNode::translate(float x, float y, float z)
{
	m_transform.translate(Eigen::Vector3f(x, y, z));
}

void XRenderNode::setPosition(float x, float y, float z)
{
	m_transform.translation() << x, y, z;
}

void XRenderNode::setScale(float x, float y, float z)
{
	auto data = XQ::Matrix::transformDecomposition_TRS(m_transform);
	data.sx = x;
	data.sy = y;
	data.sz = z;
	m_transform.matrix() = XQ::Matrix::computeMatrix(data);
}

void XRenderNode::rotate(float angle, XQ::Vec3f dir)
{
	Eigen::Vector3f axis = Eigen::Vector3f(dir.x(), dir.y(), dir.z());
	axis.normalize();
	m_transform.rotate(Eigen::AngleAxisf(XQ::Matrix::radian(angle), axis));
}

void XRenderNode::rotateX(float angle)
{
	m_transform.rotate(Eigen::AngleAxisf(XQ::Matrix::radian(angle), Eigen::Vector3f::UnitX()));
}

void XRenderNode::rotateY(float angle)
{
	m_transform.rotate(Eigen::AngleAxisf(XQ::Matrix::radian(angle), Eigen::Vector3f::UnitY()));
}

void XRenderNode::rotateZ(float angle)
{
	m_transform.rotate(Eigen::AngleAxisf(XQ::Matrix::radian(angle), Eigen::Vector3f::UnitZ()));
}

void XRenderNode::scale(float x, float y, float z)
{
	m_transform.scale(Eigen::Vector3f(x, y, z));
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

void XRenderNode::findNodeById(uint64_t id, sptr<XRenderNode>& findNode)
{
	if (isSelf(id)) {
		findNode = asDerived<XRenderNode>();
		return;
	}
		
	for (auto iter = renderNodes->begin(); iter != renderNodes->end(); iter++) {
		(*iter)->asDerived<XRenderNode>()->findNodeById(id,findNode);
		if (findNode)
		{
			return;
		}
	}
}

sptr<XRenderNode> XRenderNode::findNodeById(uint64_t id)
{
	sptr<XRenderNode>find;
	findNodeById(id,find);
	return find;
}

//XDrawableRenderNode
XDrawableRenderNode::XDrawableRenderNode()
{
}

XDrawableRenderNode::~XDrawableRenderNode()
{
}

//XRenderNode3D
XRenderNode3D::XRenderNode3D()
{
}

XRenderNode3D::~XRenderNode3D()
{
}

//XRenderNode2D
XRenderNode2D::XRenderNode2D()
{
}

XRenderNode2D::~XRenderNode2D()
{
}

//XGroupRenderNode
XGroupRenderNode::XGroupRenderNode()
{
}

XGroupRenderNode::~XGroupRenderNode()
{
}

//XTransformRenderNode
XTransformRenderNode::XTransformRenderNode()
{
}

XTransformRenderNode::~XTransformRenderNode()
{
}
