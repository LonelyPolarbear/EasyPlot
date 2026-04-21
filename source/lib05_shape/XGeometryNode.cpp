#include "XGeometryNode.h"
#include <lib04_opengl/XOpenGLBuffer.h>
#include <lib04_opengl/XOpenGLType.h>
#include <lib04_opengl/XOpenGLVertexArrayObject.h>

#include <Eigen/Eigen>
#include <lib00_utilty/XUtilty.h>
#include <dataBase/XVector.h>
#include <lib01_shader/xshaderManger.h>

#include <glew/glew.h>

#include "datasource/xshapeSource.h"
#include "XGeometryNode.h"



class XGeometryNode::Internal {
public:
	std::mutex m_mutex;
};
XGeometryNode::XGeometryNode():d(new Internal)
{
	Modified();
}

XGeometryNode::~XGeometryNode()
{
}

void XGeometryNode::draw(std::shared_ptr<xshader> shader, const Eigen::Matrix4f& parentMatrix)
{
	if(!isVisible())
		return;

	if(!shader)
		return;

	shader->use();

	Eigen::Matrix4f matrix = parentMatrix * m_transform.matrix();
	shader->setModelMatrix(matrix.data());
	shader->setObjectID(getID());
	auto preSelectColor = Attribute->AttrPreSelectColor->getValue();
	shader->setPreSelectColor(preSelectColor.r2(), preSelectColor.g2(), preSelectColor.b2(), preSelectColor.a2());
	shader->setColorMode((int)getColorMode());
	shader->setPolygonMode((int)getPolygonMode());
	auto singleColor = Attribute->AttrSingleColor->getValue();
	shader->setSingleColor(singleColor.r2(), singleColor.g2(), singleColor.b2(), singleColor.a2());

	m_polyMapper->draw(shader,getPolygonMode(),getDrawType());

}

void XGeometryNode::draw(const Eigen::Matrix4f& parentMatrix, bool isNormal)
{
	if (isNormal) {
		draw(getShaderManger()->getShader3D(getDrawType()), parentMatrix);
	}
	else {
		draw(getShaderManger()->getPickShader3D(), parentMatrix);
	}
	

	Eigen::Matrix4f matrix = parentMatrix * m_transform.matrix();
	for (auto m : *renderNodes) {
		m->asDerived<XRenderNode>()->draw(matrix,isNormal);
	}
}

const float* XGeometryNode::getMatrix() const
{
	return m_transform.matrix().data();
}

void XGeometryNode::setVisible(bool visible) {
	XRenderNode::setVisible(visible);
}

bool XGeometryNode::isVisible() {
	return XRenderNode::isVisible();
}



void XGeometryNode::setPolygonMode(PolygonMode mode)
{
	Attribute->AttrPolygonMode->setValue(mode);
}

PolygonMode XGeometryNode::getPolygonMode() const
{
	std::lock_guard<std::mutex> lock(d->m_mutex);
	return Attribute->AttrPolygonMode->getValue();
}

PrimitveType XGeometryNode::getDrawType() const
{
	return Attribute->AttrPrimitveType->getValue();
}

void XGeometryNode::setDrawType(PrimitveType type)
{
	Attribute->AttrPrimitveType->setValue(type);
}

void XGeometryNode::setColorMode(ColorMode mode)
{
	Attribute->AttrColorMode->setValue(mode);
}

ColorMode XGeometryNode::getColorMode() const
{
	std::lock_guard<std::mutex> lock(d->m_mutex);
	return Attribute->AttrColorMode->getValue();
}

void XGeometryNode::setSingleColor(XQ::Vec4f color)
{
	Attribute->AttrSingleColor->setValue(XQ::XColor::from_normalcolor(color) );
}

XQ::Vec4f XGeometryNode::getSingleColor() const
{
	auto tmp = Attribute->AttrSingleColor->getValue();
	return XQ::Vec4f(tmp.r2(), tmp.g2(), tmp.b2(), tmp.a());
}

void XGeometryNode::setPreSelectColor(XQ::Vec4f color) {
	Attribute->AttrPreSelectColor->setValue(XQ::XColor::from_normalcolor(color));
}

XQ::Vec4f XGeometryNode::getPreSelectColor() const {
	auto tmp = Attribute->AttrPreSelectColor->getValue();
	return XQ::Vec4f(tmp.r2(), tmp.g2(), tmp.b2(), tmp.a());
}

XQ::BoundBox XGeometryNode::getThisBoundBox(const Eigen::Matrix4f& m) const
{
	Eigen::Affine3f t;
	t.matrix() = m*m_transform.matrix();
	return m_polyMapper->getInput()->getBoundBox(t);
}

void XGeometryNode::setPolyDataMapper(sptr<XPolyDataMapper> mapper)
{
	m_polyMapper = mapper;
}

sptr<XPolyDataMapper> XGeometryNode::getPolyDataMapper() const
{
	return m_polyMapper;
}

sptr<XPolyDataMapper> XGeometryNode::getOrCreateMapper()
{
	if (getPolyDataMapper()) {
		return getPolyDataMapper();
	}
	else {
		setPolyDataMapper(makeShareDbObject<XPolyDataMapper>());
		return getPolyDataMapper();
	}
}

void XGeometryNode::setInput(sptr<XShapeSource> input)
{
	getOrCreateMapper()->setInput(input);
}

void XGeometryNode::Init()
{
	XRenderNode3D::Init();
	XQ_XDATA_ADD(Attribute);
	//Attribute = makeShareDbObject<XRenderNode3DAttribute>();
}