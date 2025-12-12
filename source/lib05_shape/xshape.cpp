#include "xshape.h"
#include <lib04_opengl/XOpenGLBuffer.h>
#include <lib04_opengl/XOpenGLType.h>
#include <lib04_opengl/XOpenGLVertexArrayObject.h>

#include <Eigen/Eigen>
#include <lib00_utilty/myUtilty.h>
#include <lib01_shader/xshaderManger.h>

#include <glew/glew.h>

#include "xshapeSource.h"

static std::atomic< uint64_t>  object_id_counter(0);

class XShape::Internal {
public:
	Eigen::Affine3f m_transform = Eigen::Affine3f::Identity();
	std::mutex m_mutex;
	uint64_t m_id = ++object_id_counter;
	bool isInitResource = false;
};
XShape::XShape():d(new Internal)
{
	m_vao = makeShareDbObject<XOpenGLVertexArrayObject>();
	m_vbo_coord = makeShareDbObject<XOpenGLBuffer>();
	m_ebo = makeShareDbObject<XOpenGLBuffer>();
	m_ssbo_color = makeShareDbObject<XOpenGLBuffer>();

	m_selectTestColor = computeSelectTestColor();
	Modified();
}

XShape::~XShape()
{
}

void XShape::draw(std::shared_ptr<xshader> shader)
{
	initiallize();

	//先更新数据
	updateData();
	if(!m_visible)
		return;

	if(!shader)
		return;

	shader->use();

	shader->setModelMatrix(this->getMatrix());
	shader->setObjectID(d->m_id);
	shader->setPreSelectColor(m_preSelectColor.x(), m_preSelectColor.y(), m_preSelectColor.z(), m_preSelectColor.w());
	shader->setColorMode((int)m_colorMode);
	shader->setPolygonMode((int)m_polygonMode);
	shader->setSingleColor(m_singleColor.x(), m_singleColor.y(), m_singleColor.z(), m_singleColor.w());
	shader->setSelectTestColor(m_selectTestColor.x(), m_selectTestColor.y(), m_selectTestColor.z(), m_selectTestColor.w());

	bindSSBO();

	m_vao->bind();
	auto index= m_Input->getIndexArray();
	if (m_polygonMode == PolygonMode::fill) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDrawElements((unsigned int)m_drawType, index->getNumOfTuple() * index->getComponent(), GL_UNSIGNED_INT, 0);
	}else if (m_polygonMode == PolygonMode::line) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glPolygonOffset( 1.0f, 1.0f );
		glDrawElements((unsigned int)m_drawType, index->getNumOfTuple() * index->getComponent(), GL_UNSIGNED_INT, 0);
	}else if (m_polygonMode == PolygonMode::line_fill) {

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDrawElements((unsigned int)m_drawType, index->getNumOfTuple() * index->getComponent(), GL_UNSIGNED_INT, 0);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glPolygonOffset(1.0f, 1.0f);
		glDrawElements((unsigned int)m_drawType, index->getNumOfTuple() * index->getComponent(), GL_UNSIGNED_INT, 0);
	}
	
	m_vao->release();
}

void XShape::draw()
{
	initiallize();
	auto shader = m_shaderManger->getShader3D(m_drawType);
	draw(shader);
}

void XShape::initResource()
{
	m_vao->create();
	m_vao->bind();

	m_vbo_coord->setBufferType(XOpenGLBuffer::VertexBuffer);

	m_vbo_coord->setUsagePattern(XOpenGLBuffer::StaticDraw);

	m_vbo_coord->create();

	m_vbo_coord->bind();

	m_ssbo_color->setBufferType(XOpenGLBuffer::ShaderStorageBuffer);

	m_ssbo_color->setUsagePattern(XOpenGLBuffer::StaticDraw);

	m_ssbo_color->create();

	m_ssbo_color->bind();

	//设置顶点属性
	
	m_vao->addBuffer(0, m_vbo_coord, 3, XOpenGL::DataType::float_, sizeof(XQ::Vec3f), 0);

	m_ebo->setBufferType(XOpenGLBuffer::IndexBuffer);

	m_ebo->setUsagePattern(XOpenGLBuffer::StaticDraw);

	m_ebo->create();

	m_ebo->bind();

	m_vao->bind();

	m_ebo->bind();

	m_vbo_coord->release();

	m_vao->release();
}

void XShape::initiallize()
{
	if (d->isInitResource == false) {
		initResource();
		d->isInitResource = true;
	}
}


int64_t XShape::getID() const
{
	return d->m_id;
}

void XShape::bindSSBO()
{
	m_ssbo_color->bind();
	m_ssbo_color->setBufferBindIdx(0);
}

void XShape::translate(float x, float y, float z)
{
	d->m_transform.translate(Eigen::Vector3f(x, y, z));
}

void XShape::setPosition(float x, float y, float z)
{
	d->m_transform.translation()<<x,y,z;
}

void XShape::setScale(float x, float y, float z)
{
	auto data = XQ::Matrix::transformDecomposition_TRS(d->m_transform);
	data.sx = x;
	data.sy = y;
	data.sz = z;
	d->m_transform.matrix() = XQ::Matrix::computeMatrix(data);
}

void XShape::rotate(float angle, XQ::Vec3f dir)
{
	Eigen::Vector3f axis = Eigen::Vector3f(dir.x(), dir.y(), dir.z());
	axis.normalize();
	d->m_transform.rotate(Eigen::AngleAxisf(XQ::Matrix::radian(angle), axis));
}

float* XShape::getMatrix() const
{
	return d->m_transform.matrix().data();
}

void XShape::setVisible(bool visible) {
	m_visible = visible;
}

bool XShape::isVisible() {
	return m_visible;
}

void XShape::rotateX(float angle)
{
	d->m_transform.rotate(Eigen::AngleAxisf(XQ::Matrix::radian(angle), Eigen::Vector3f::UnitX()));
}

void XShape::rotateY(float angle)
{
	d->m_transform.rotate(Eigen::AngleAxisf(XQ::Matrix::radian(angle), Eigen::Vector3f::UnitY()));
}

void XShape::rotateZ(float angle)
{
	d->m_transform.rotate(Eigen::AngleAxisf(XQ::Matrix::radian(angle), Eigen::Vector3f::UnitZ()));
}

void XShape::scale(float x, float y, float z)
{
	d->m_transform.scale(Eigen::Vector3f(x, y, z));
}

void XShape::setPolygonMode(PolygonMode mode)
{
	m_polygonMode = mode;
}

PolygonMode XShape::getPolygonMode() const
{
	std::lock_guard<std::mutex> lock(d->m_mutex);
	return m_polygonMode;
}

void XShape::setColorMode(ColorMode mode)
{
	m_colorMode = mode;
}

ColorMode XShape::getColorMode() const
{
	std::lock_guard<std::mutex> lock(d->m_mutex);
	return m_colorMode;
}

void XShape::setSingleColor(XQ::Vec4f color)
{
	m_singleColor = color;
}

XQ::Vec4f XShape::getSingleColor() const
{
	return m_singleColor;
}

void XShape::setPreSelectColor(XQ::Vec4f color) {
	m_preSelectColor = color;
}

XQ::Vec4f XShape::getPreSelectColor() const {
	return m_preSelectColor;
}

XQ::Vec4f XShape::computeSelectTestColor()
{
	int32_t id = d->m_id;

	int32_t a = (id >> 24) & 0xff;
	int32_t b = (id >> 16) & 0xff;
	int32_t g = (id >> 8) & 0xff;
	int32_t r = id & 0xff;
	a = 255;

	return XQ::Vec4f((float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f, (float)a / 255.0f);

}

uint64_t XShape::colorToUInt(XQ::Vec4f color)
{
	uint64_t a = (uint64_t)(color.x() * 255.0f);
	uint64_t b = (uint64_t)(color.y() * 255.0f);
	uint64_t g = (uint64_t)(color.z() * 255.0f);
	uint64_t r = (uint64_t)(color.w() * 255.0f);
	return (r << 24) | (g << 16) | (b << 8) | a;
}

bool XShape::isSelf(uint64_t id)
{
	return d->m_id == id;
}

void XShape::setShaderManger(std::shared_ptr<xShaderManger> shaderManger) {
	m_shaderManger = shaderManger;
}

std::shared_ptr<xShaderManger> XShape::getShaderManger() const {
	return m_shaderManger;
}

std::shared_ptr<XFloatArray> XShape::getCoordAarray() const
{
	return m_Input->getVertextCoordArray();
}

std::shared_ptr<XFloatArray> XShape::getNormalArray() const
{
	return m_Input->getNormalArray();
}

std::shared_ptr<XFloatArray> XShape::getVertexColorArray() const
{
	return m_Input->getVertexColorArray();
}

std::shared_ptr<XFloatArray> XShape::getFaceColorArray() const
{
	return m_Input->getFaceColorArray();;
}

std::shared_ptr<XUIntArray> XShape::getIndexsArray() const
{
	return m_Input->getIndexArray();;
}

void XShape::updateData()
{
	std::lock_guard<std::mutex> lock(d->m_mutex);
	if (!m_Input->update()) {
		return;
	}
	
	//顶点数据已经更新
	auto m_coord = m_Input->getVertextCoordArray();
	if (m_coord && m_coord->GetTimeStamp() > m_UpdateTime) {

		m_vbo_coord->bind();   

		m_vbo_coord->allocate(m_coord->data(0), m_coord->size());

		m_vbo_coord->release();
	}

	//索引数据已经更新
	auto m_indexs = m_Input->getIndexArray();
	if (m_indexs && m_indexs->GetTimeStamp() > m_UpdateTime) {
		//可能需要先激活VAO
		m_vao->bind();

		m_ebo->bind();	

		m_ebo->allocate(m_indexs->data(0), m_indexs->size());

		m_vao->release();

		m_ebo->release();
	}

	//法向量数据已经更新
	auto m_normal = m_Input->getNormalArray();
	if (m_normal && m_normal->GetTimeStamp() > m_UpdateTime) {
		
	}

	////顶点颜色数据已经更新
	auto m_VertexColor = m_Input->getVertexColorArray();
	if (m_VertexColor && m_VertexColor->GetTimeStamp() > m_UpdateTime) {

	}

	//面颜色数据已经更新
	auto m_FaceColor = m_Input->getFaceColorArray();
	if (m_FaceColor && m_FaceColor->GetTimeStamp() > m_UpdateTime) {
		m_ssbo_color->bind();
		m_ssbo_color->allocate(m_FaceColor->data(0), m_FaceColor->size());
		m_ssbo_color->release();
	}

	//数据已更新，刷新时间戳
	m_UpdateTime.Modified();
}

void XShape::setInput(std::shared_ptr<XShapeSource> input)
{
	m_Input = input;
}

XQ::BoundBox XShape::getBoundBox() const
{
	return m_Input->getBoundBox(d->m_transform);
}
