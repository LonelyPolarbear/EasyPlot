#include "XPolyDataMapper.h"
#include <lib04_opengl/XOpenGLBuffer.h>
#include <lib04_opengl/XOpenGLType.h>
#include <lib04_opengl/XOpenGLVertexArrayObject.h>
#include <lib01_shader/xshader.h>
#include <glew/glew.h>
#include "../datasource/xshapeSource.h"

#define ATTR_VERTER_COORD 0
#define ATTR_VERTEX_NORMAL 1
#define ATTR_VERTEX_COLOR 2
#define ATTR_VERTEX_TEXTURE_COORD 3

#define SSBO_FACE_COLOR 0
#define SSBO_LINE_COLOR 1

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

	//!
	//! [1] 顶点坐标数据更新
	auto vertex_coord = m_Input->getVertextCoordArray();
	if (vertex_coord && vertex_coord->GetTimeStamp() > m_UpdateTime) {

		m_vertex_coord->bind();

		m_vertex_coord->allocate(vertex_coord->data(0), vertex_coord->size());

		m_vertex_coord->release();

		if (vertex_coord->getNumOfTuple()) {
			m_vao->enableAttribute(ATTR_VERTER_COORD);
		}
		else {
			m_vao->disableAttribute(ATTR_VERTER_COORD);
		}
	}

	//!
	//! [2] 顶点法向量数据更新
	auto vertex_normal = m_Input->getVertexNormalArray();
	if (vertex_normal && vertex_normal->GetTimeStamp() > m_UpdateTime) {
		m_vertex_normal->bind();

		m_vertex_normal->allocate(vertex_normal->data(0), vertex_normal->size());

		m_vertex_normal->release();

		if (vertex_normal->getNumOfTuple()) {
			m_vao->enableAttribute(ATTR_VERTEX_NORMAL);
		}
		else {
			m_vao->disableAttribute(ATTR_VERTEX_NORMAL);
		}
	}

	//!
	//! [3] 顶点颜色数据更新
	auto vertex_color = m_Input->getVertexColorArray();
	if (vertex_color && vertex_color->GetTimeStamp() > m_UpdateTime) {
		m_vertex_color->bind();

		m_vertex_color->allocate(vertex_color->data(0), vertex_color->size());

		m_vertex_color->release();

		if (vertex_color->getNumOfTuple()) {
			m_vao->addBuffer(ATTR_VERTEX_COLOR, m_vertex_color, 3, XOpenGL::DataType::float_, sizeof(XQ::Vec3f), 0);
			m_vao->enableAttribute(ATTR_VERTEX_COLOR);
		}
		else {
			m_vao->disableAttribute(ATTR_VERTEX_COLOR);
		}
	}

	//!
	//! [4] 面索引数据更新
	auto face_indexs = m_Input->getFaceIndexArray();
	if (face_indexs && face_indexs->GetTimeStamp() > m_UpdateTime) {
		//可能需要先激活VAO
		m_vao->bind();

		m_face_ebo->bind();

		m_face_ebo->allocate(face_indexs->data(0), face_indexs->size());

		m_vao->release();

		m_face_ebo->release();
	}

	//!
	//! [5] 面颜色数据更新
	auto face_color = m_Input->getFaceColorArray();
	if (face_color && face_color->GetTimeStamp() > m_UpdateTime) {
		m_face_color->bind();
		m_face_color->allocate(face_color->data(0), face_color->size());
		m_face_color->release();
	}

	//!
	//! [6] 线索引数据更新
	auto line_indexs = m_Input->getLineIndexArray();
	if (line_indexs && line_indexs->GetTimeStamp() > m_UpdateTime) {
		//可能需要先激活VAO
		m_vao->bind();

		m_line_ebo->bind();

		m_line_ebo->allocate(line_indexs->data(0), line_indexs->size());

		m_vao->release();

		m_line_ebo->release();
	}

	//!
	//! [7] 线颜色数据更新
	auto line_color = m_Input->getLineColorArray();
	if (line_color && line_color->GetTimeStamp() > m_UpdateTime) {
		m_line_color->bind();
		m_line_color->allocate(line_color->data(0), line_color->size());
		m_line_color->release();
	}

	//!
	//! [8] 点索引数据更新
	auto vertex_indexs = m_Input->getVertexIndexArray();
	if (vertex_indexs && vertex_indexs->GetTimeStamp() > m_UpdateTime) {
		//可能需要先激活VAO
		m_vao->bind();

		m_point_ebo->bind();

		m_point_ebo->allocate(vertex_indexs->data(0), vertex_indexs->size());

		m_vao->release();

		m_point_ebo->release();
	}

	//!
	//! [9] 纹理坐标
	auto texture_coord = m_Input->getTextureCoordArray();
	if (texture_coord && texture_coord->GetTimeStamp() > m_UpdateTime) {
		m_vertex_textureCoord->bind();

		m_vertex_textureCoord->allocate(texture_coord->data(0), texture_coord->size());

		m_vertex_textureCoord->release();

		if (texture_coord->getNumOfTuple()) {
			m_vao->addBuffer(ATTR_VERTEX_TEXTURE_COORD, m_vertex_color, 2, XOpenGL::DataType::float_, sizeof(XQ::Vec2f), 0);
			m_vao->enableAttribute(ATTR_VERTEX_TEXTURE_COORD);
		}
		else {
			m_vao->disableAttribute(ATTR_VERTEX_TEXTURE_COORD);
		}
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
	auto face_index_num = m_Input->getFaceIndexArray()->size();
	auto line_index_num = m_Input->getLineIndexArray()->size();
	auto point_index_num = m_Input->getVertexIndexArray()->size();

	auto hasMode=[&](PolygonMode mode)->bool{
		return (uint32_t)polygonMode & (uint32_t)mode;
	};

	if (hasMode(PolygonMode::face)) {
		//绑定对应的ebo
		shader->setPolygonMode((int)PolygonMode::face);
		m_face_ebo->bind();
		glDrawElements((unsigned int)drawType, face_index_num, GL_UNSIGNED_INT, 0);
	}

	if (hasMode(PolygonMode::line)) {
		//绑定对应的ebo
		//glEnable(GL_POLYGON_OFFSET_LINE);
		//glPolygonOffset(0.0f, -0.01f);  // 负值使线更靠近相机
		shader->setPolygonMode((int)PolygonMode::line);
		m_line_ebo->bind();
		glDrawElements((unsigned int)(PrimitveType::line), line_index_num, GL_UNSIGNED_INT, 0);
	}

	if (hasMode(PolygonMode::point)) {
		//绑定对应的ebo
		//glEnable(GL_POLYGON_OFFSET_POINT);
		//glPolygonOffset(0.0f, -0.02f);
		glPointSize(5);
		shader->setPolygonMode((int)PolygonMode::point);
		m_point_ebo->bind();
		glDrawElements((unsigned int)(PrimitveType::point), point_index_num, GL_UNSIGNED_INT, 0);
	}

	m_vao->release();
	/**************/
	//ebo是局部状态，隶属于vao，当vao解绑后，如果读取当前绑定的ebo,已经是0，不需要release了
	//m_face_ebo->release();
	//m_line_ebo->release();
	//m_point_ebo->release();
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
	//!
	//! [1] VAO 初始化
	//! 
	m_vao->create();

	m_vao->bind();

	//!
	//! [2] VBO 初始化
	//! 
	{
		m_vertex_coord->setBufferType(XOpenGLBuffer::VertexBuffer);

		m_vertex_coord->setUsagePattern(XOpenGLBuffer::StaticDraw);

		m_vertex_coord->create();

		m_vertex_normal->setBufferType(XOpenGLBuffer::VertexBuffer);

		m_vertex_normal->setUsagePattern(XOpenGLBuffer::StaticDraw);

		m_vertex_normal->create();

		m_vertex_color->setBufferType(XOpenGLBuffer::VertexBuffer);

		m_vertex_color->setUsagePattern(XOpenGLBuffer::StaticDraw);

		m_vertex_color->create();

		m_vertex_textureCoord->setBufferType(XOpenGLBuffer::VertexBuffer);

		m_vertex_textureCoord->setUsagePattern(XOpenGLBuffer::StaticDraw);

		m_vertex_textureCoord->create();

	}


	//!
	//! [3] SSBO 初始化
	//!
	{
		m_face_color->setBufferType(XOpenGLBuffer::ShaderStorageBuffer);

		m_face_color->setUsagePattern(XOpenGLBuffer::StaticDraw);

		m_face_color->create();

		m_line_color->setBufferType(XOpenGLBuffer::ShaderStorageBuffer);

		m_line_color->setUsagePattern(XOpenGLBuffer::StaticDraw);

		m_line_color->create();
	}
	

	//!
	//! [4] 设置顶点属性
	//!
	m_vao->addBuffer(ATTR_VERTER_COORD, m_vertex_coord, 3, XOpenGL::DataType::float_, sizeof(XQ::Vec3f), 0);

	//!
	//! [5] ebo的创建和关联
	{
		m_face_ebo->setBufferType(XOpenGLBuffer::IndexBuffer);

		m_face_ebo->setUsagePattern(XOpenGLBuffer::StaticDraw);

		m_face_ebo->create();

		m_line_ebo->setBufferType(XOpenGLBuffer::IndexBuffer);

		m_line_ebo->setUsagePattern(XOpenGLBuffer::StaticDraw);

		m_line_ebo->create();

		m_point_ebo->setBufferType(XOpenGLBuffer::IndexBuffer);

		m_point_ebo->setUsagePattern(XOpenGLBuffer::StaticDraw);

		m_point_ebo->create();

		//每个vao同一时刻只能绑定一个ebo
		//m_face_ebo->bind();

		m_vao->bind();

		m_face_ebo->bind();

		m_vao->release();

		m_face_ebo->release();				//ebo的解绑应该在vao解绑之后，否则相当于vao绑定到了index=0的ebo了
	}
}

void XPolyDataMapper::bindSSBO()
{
	m_face_color->bind();
	m_face_color->setBufferBindIdx(SSBO_FACE_COLOR);

	m_line_color->bind();
	m_line_color->setBufferBindIdx(SSBO_LINE_COLOR);
}

void XPolyDataMapper::Init()
{
	m_vao = makeShareDbObject<XOpenGLVertexArrayObject>();

	m_vertex_coord = makeShareDbObject<XOpenGLBuffer>();
	m_vertex_normal = makeShareDbObject<XOpenGLBuffer>();
	m_vertex_color = makeShareDbObject<XOpenGLBuffer>();
	m_vertex_textureCoord = makeShareDbObject<XOpenGLBuffer>();

	m_face_ebo = makeShareDbObject<XOpenGLBuffer>();
	m_face_color = makeShareDbObject<XOpenGLBuffer>();

	m_line_ebo = makeShareDbObject<XOpenGLBuffer>();
	m_line_color = makeShareDbObject<XOpenGLBuffer>();

	m_point_ebo = makeShareDbObject<XOpenGLBuffer>();
}