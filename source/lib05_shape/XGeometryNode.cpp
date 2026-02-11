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
	Eigen::Affine3f m_transform = Eigen::Affine3f::Identity();
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
	if(!m_visible)
		return;

	if(!shader)
		return;

	shader->use();

	Eigen::Matrix4f matrix = parentMatrix * d->m_transform.matrix();
	shader->setModelMatrix(matrix.data());
	shader->setObjectID(getID());
	shader->setPreSelectColor(m_preSelectColor.x(), m_preSelectColor.y(), m_preSelectColor.z(), m_preSelectColor.w());
	shader->setColorMode((int)m_colorMode);
	shader->setPolygonMode((int)m_polygonMode);
	shader->setSingleColor(m_singleColor.x(), m_singleColor.y(), m_singleColor.z(), m_singleColor.w());

	m_polyMapper->draw(shader,m_polygonMode,m_drawType);
}

void XGeometryNode::draw(const Eigen::Matrix4f& parentMatrix)
{
	draw(m_shaderManger->getShader3D(m_drawType),parentMatrix);
}

void XGeometryNode::translate(float x, float y, float z)
{
	d->m_transform.translate(Eigen::Vector3f(x, y, z));
}

void XGeometryNode::setPosition(float x, float y, float z)
{
	d->m_transform.translation()<<x,y,z;
}

void XGeometryNode::setScale(float x, float y, float z)
{
	auto data = XQ::Matrix::transformDecomposition_TRS(d->m_transform);
	data.sx = x;
	data.sy = y;
	data.sz = z;
	d->m_transform.matrix() = XQ::Matrix::computeMatrix(data);
}

void XGeometryNode::rotate(float angle, XQ::Vec3f dir)
{
	Eigen::Vector3f axis = Eigen::Vector3f(dir.x(), dir.y(), dir.z());
	axis.normalize();
	d->m_transform.rotate(Eigen::AngleAxisf(XQ::Matrix::radian(angle), axis));
}

float* XGeometryNode::getMatrix() const
{
	return d->m_transform.matrix().data();
}

void XGeometryNode::setVisible(bool visible) {
	m_visible = visible;
}

bool XGeometryNode::isVisible() {
	return m_visible;
}

void XGeometryNode::rotateX(float angle)
{
	d->m_transform.rotate(Eigen::AngleAxisf(XQ::Matrix::radian(angle), Eigen::Vector3f::UnitX()));
}

void XGeometryNode::rotateY(float angle)
{
	d->m_transform.rotate(Eigen::AngleAxisf(XQ::Matrix::radian(angle), Eigen::Vector3f::UnitY()));
}

void XGeometryNode::rotateZ(float angle)
{
	d->m_transform.rotate(Eigen::AngleAxisf(XQ::Matrix::radian(angle), Eigen::Vector3f::UnitZ()));
}

void XGeometryNode::scale(float x, float y, float z)
{
	d->m_transform.scale(Eigen::Vector3f(x, y, z));
}

void XGeometryNode::setPolygonMode(PolygonMode mode)
{
	m_polygonMode = mode;
}

PolygonMode XGeometryNode::getPolygonMode() const
{
	std::lock_guard<std::mutex> lock(d->m_mutex);
	return m_polygonMode;
}

void XGeometryNode::setColorMode(ColorMode mode)
{
	m_colorMode = mode;
}

ColorMode XGeometryNode::getColorMode() const
{
	std::lock_guard<std::mutex> lock(d->m_mutex);
	return m_colorMode;
}

void XGeometryNode::setSingleColor(XQ::Vec4f color)
{
	m_singleColor = color;
}

XQ::Vec4f XGeometryNode::getSingleColor() const
{
	return m_singleColor;
}

void XGeometryNode::setPreSelectColor(XQ::Vec4f color) {
	m_preSelectColor = color;
}

XQ::Vec4f XGeometryNode::getPreSelectColor() const {
	return m_preSelectColor;
}

//uint64_t XGeometryNode::colorToUInt(XQ::Vec4f color)
//{
//	uint64_t a = (uint64_t)(color.x() * 255.0f);
//	uint64_t b = (uint64_t)(color.y() * 255.0f);
//	uint64_t g = (uint64_t)(color.z() * 255.0f);
//	uint64_t r = (uint64_t)(color.w() * 255.0f);
//	return (r << 24) | (g << 16) | (b << 8) | a;
//}

bool XGeometryNode::isSelf(uint64_t id)
{
	return getID() == id;
}

void XGeometryNode::setShaderManger(std::shared_ptr<xShaderManger> shaderManger) {
	m_shaderManger = shaderManger;
}

std::shared_ptr<xShaderManger> XGeometryNode::getShaderManger() const {
	return m_shaderManger;
}


XQ::BoundBox XGeometryNode::getBoundBox() const
{
	return m_polyMapper->getInput()->getBoundBox(d->m_transform);
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