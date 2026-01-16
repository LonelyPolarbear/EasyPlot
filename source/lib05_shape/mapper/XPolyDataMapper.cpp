#include "XPolyDataMapper.h"
#include <lib04_opengl/XOpenGLBuffer.h>
#include <lib04_opengl/XOpenGLType.h>
#include <lib04_opengl/XOpenGLVertexArrayObject.h>
#include <glew/glew.h>
#include "../datasource/xshapeSource.h"

XPolyDataMapper::XPolyDataMapper()
{
}

XPolyDataMapper::~XPolyDataMapper()
{
}

void XPolyDataMapper::setInput(std::shared_ptr<XShapeSource> input)
{
	m_Input = input;
}

void XPolyDataMapper::updateData()
{
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

void XPolyDataMapper::draw(sptr<xshader> shader, PolygonMode polygonMode, PrimitveType drawType)
{
	initiallize();

	bindSSBO();

	updateData();

	m_vao->bind();
	auto index = m_Input->getIndexArray();
	if (polygonMode == PolygonMode::fill) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDrawElements((unsigned int)drawType, index->getNumOfTuple() * index->getComponent(), GL_UNSIGNED_INT, 0);
	}
	else if (polygonMode == PolygonMode::line) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glPolygonOffset(1.0f, 1.0f);
		glDrawElements((unsigned int)drawType, index->getNumOfTuple() * index->getComponent(), GL_UNSIGNED_INT, 0);
	}
	else if (polygonMode == PolygonMode::point) {
		
	}

	m_vao->release();
}

void XPolyDataMapper::initiallize()
{
	if (!isGLResourceInit) {
		initGLResource();
		isGLResourceInit = true;
	}
}

void XPolyDataMapper::initGLResource()
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

void XPolyDataMapper::bindSSBO()
{
	m_ssbo_color->bind();
	m_ssbo_color->setBufferBindIdx(0);
}

void XPolyDataMapper::Init()
{
	m_vao = makeShareDbObject<XOpenGLVertexArrayObject>();
	m_vbo_coord = makeShareDbObject<XOpenGLBuffer>();
	m_ebo = makeShareDbObject<XOpenGLBuffer>();
	m_ssbo_color = makeShareDbObject<XOpenGLBuffer>();
}
