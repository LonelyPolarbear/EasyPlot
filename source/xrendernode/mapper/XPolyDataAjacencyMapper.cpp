#include "XPolyDataAjacencyMapper.h"
#include <xrendernode/datasource/xshapeSource.h>
#include <lib04_opengl/XOpenGLBuffer.h>
#include <lib04_opengl/XOpenGLVertexArrayObject.h>
#include <glew/glew.h>
#define ATTR_VERTER_COORD 0
#define ATTR_VERTEX_NORMAL 1
#define ATTR_VERTEX_COLOR 2
#define ATTR_VERTEX_TEXTURE_COORD 3
#define ATTR_VERTEX_INSTANCED_MAT 4

#define SSBO_FACE_COLOR 0
#define SSBO_LINE_COLOR 1

class XPolyDataAjacencyMapper::Internal {
public:
	std::shared_ptr<XOpenGLBuffer> m_ssbo_len;
};

XPolyDataAjacencyMapper::XPolyDataAjacencyMapper():mData(new Internal)
{

}

XPolyDataAjacencyMapper::~XPolyDataAjacencyMapper()
{

}

void XPolyDataAjacencyMapper::Init()
{
	XPolyDataMapper::Init();

	mData->m_ssbo_len = makeShareDbObject<XOpenGLBuffer>();
}

void XPolyDataAjacencyMapper::bindSSBO()
{
	XPolyDataMapper::bindSSBO();

	mData->m_ssbo_len->bind();
	mData->m_ssbo_len->setBufferBindIdx(5/*todo*/);
}

void XPolyDataAjacencyMapper::InitRenderResource()
{
	XPolyDataMapper::InitRenderResource();

	mData->m_ssbo_len->setBufferType(XOpenGLBuffer::ShaderStorageBuffer);

	mData->m_ssbo_len->setUsagePattern(XOpenGLBuffer::StaticDraw);

	mData->m_ssbo_len->create();

	mData->m_ssbo_len->bind();
}

void XPolyDataAjacencyMapper::updateData()
{
	if (!m_Input->update()) {
		return;
	}

	//!
	//! [1] 顶点坐标数据更新
	auto vertex_coord = m_Input->getVertextCoordArray();
	if (vertex_coord && vertex_coord->GetTimeStamp() > m_UpdateTime) {

		m_vertex_coord->bind();

		//m_vertex_coord->allocate(vertex_coord->data(0), vertex_coord->size());
		m_vertex_coord->allocate( (vertex_coord->size()+6)*sizeof(float));//额外增加两个点

		auto point_num = vertex_coord->getNumOfTuple();
		float* first_point = vertex_coord->data(0);
		float* last_point = vertex_coord->data(point_num-1);

		m_vertex_coord->write(0, first_point, 3);																	//插入第一个点
		m_vertex_coord->write(3, vertex_coord->data(0), vertex_coord->size());				//拷贝原始点
		m_vertex_coord->write((point_num+1)*3, last_point, 3);											//插入最后一个点
		auto dd = m_vertex_coord->map2cpu();
		auto g = dd->heteroCast<float>(1);
		m_vertex_coord->release();

		if (vertex_coord->getNumOfTuple()) {
			m_vao->enableAttribute(ATTR_VERTER_COORD);
		}
		else {
			m_vao->disableAttribute(ATTR_VERTER_COORD);
		}

		//设置曲线长度
		{
			int oldNum = vertex_coord->getNumOfTuple();
			auto lens = makeShareDbObject<XFloatArray>();
			lens->setNumOfTuple(oldNum);
			lens->setTuple(0, 0);
			for (int i = 1; i < oldNum; i++) {
				auto lastPoint = vertex_coord->data(i-1);		
				auto curPoint = vertex_coord->data(i);

				Eigen::Vector2f v1(lastPoint[0], lastPoint[1]);
				Eigen::Vector2f v2(curPoint[0], curPoint[1]);
				Eigen::Vector2f v3 = v2 - v1;
				float len = v3.norm();
				lens->setTuple(i, len + *lens->data(i - 1));
			}
			mData->m_ssbo_len->bind();
			mData->m_ssbo_len->allocate(lens->data(0), lens->size());
			mData->m_ssbo_len->release();
		}
	}

	//!
	//! [2] 顶点法向量数据更新
	auto vertex_normal = m_Input->getVertexNormalArray();
	if (vertex_normal && vertex_normal->GetTimeStamp() > m_UpdateTime) {
		m_vertex_normal->bind();

		//m_vertex_normal->allocate(vertex_normal->data(0), vertex_normal->size());
		m_vertex_normal->allocate((vertex_normal->size() + 6) * sizeof(float));//额外增加两个点

		auto point_num = vertex_normal->getNumOfTuple();
		float* first_point = vertex_normal->data(0);
		float* last_point = vertex_normal->data(point_num - 1);

		m_vertex_normal->write(0, first_point, 3);																	//插入第一个点
		m_vertex_normal->write(1, vertex_normal->data(0), vertex_normal->size());				//拷贝原始点
		m_vertex_normal->write((point_num + 1) * 3, last_point, 3);											//插入最后一个点

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

		//m_vertex_color->allocate(vertex_color->data(0), vertex_color->size());
		m_vertex_color->allocate((vertex_color->size() + 6) * sizeof(float));//额外增加两个点

		auto point_num = vertex_color->getNumOfTuple();
		float* first_point = vertex_color->data(0);
		float* last_point = vertex_color->data(point_num - 1);

		m_vertex_color->write(0, first_point, 3);																	//插入第一个点
		m_vertex_color->write(1, vertex_color->data(0), vertex_color->size());					//拷贝原始点
		m_vertex_color->write((point_num + 1) * 3, last_point, 3);											//插入最后一个点

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
	//! [4] 顶点实例化位置属性
	auto vertex_instanced = m_Input->getInstancedArray();
	if (vertex_instanced && vertex_instanced->GetTimeStamp() > m_UpdateTime) {
		m_vertex_InstancedMat->bind();

		m_vertex_InstancedMat->allocate(vertex_instanced->data(0), vertex_instanced->size());

		m_vertex_InstancedMat->release();

		if (vertex_instanced->getNumOfTuple()) {
			m_vao->addBuffer(ATTR_VERTEX_INSTANCED_MAT + 0, m_vertex_InstancedMat, 4, XOpenGL::DataType::float_, sizeof(float) * 16, 0, true);
			m_vao->addBuffer(ATTR_VERTEX_INSTANCED_MAT + 1, m_vertex_InstancedMat, 4, XOpenGL::DataType::float_, sizeof(float) * 16, sizeof(float) * 4, true);
			m_vao->addBuffer(ATTR_VERTEX_INSTANCED_MAT + 2, m_vertex_InstancedMat, 4, XOpenGL::DataType::float_, sizeof(float) * 16, sizeof(float) * 8, true);
			m_vao->addBuffer(ATTR_VERTEX_INSTANCED_MAT + 3, m_vertex_InstancedMat, 4, XOpenGL::DataType::float_, sizeof(float) * 16, sizeof(float) * 12, true);

			m_vao->enableAttribute(ATTR_VERTEX_INSTANCED_MAT + 0);
			m_vao->enableAttribute(ATTR_VERTEX_INSTANCED_MAT + 1);
			m_vao->enableAttribute(ATTR_VERTEX_INSTANCED_MAT + 2);
			m_vao->enableAttribute(ATTR_VERTEX_INSTANCED_MAT + 3);
		}
		else {
			m_vao->disableAttribute(ATTR_VERTEX_INSTANCED_MAT + 0);
			m_vao->disableAttribute(ATTR_VERTEX_INSTANCED_MAT + 1);
			m_vao->disableAttribute(ATTR_VERTEX_INSTANCED_MAT + 2);
			m_vao->disableAttribute(ATTR_VERTEX_INSTANCED_MAT + 3);
		}
	}


	//!
	//! [7] 线颜色数据更新
	auto line_color = m_Input->getLineColorArray();
	if (line_color && line_color->GetTimeStamp() > m_UpdateTime) {
		m_line_color->bind();
		m_line_color->allocate(line_color->size()+2);
		m_line_color->write(1,line_color->data(0), line_color->size());
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

		auto component = texture_coord->getComponent();

		if (texture_coord->getNumOfTuple()) {
			m_vao->addBuffer(ATTR_VERTEX_TEXTURE_COORD, m_vertex_textureCoord, component, XOpenGL::DataType::float_, sizeof(XQ::Vec2f), 0);
			m_vao->enableAttribute(ATTR_VERTEX_TEXTURE_COORD);
		}
		else {
			m_vao->disableAttribute(ATTR_VERTEX_TEXTURE_COORD);
		}
	}
}

void XPolyDataAjacencyMapper::draw(sptr<xshader> shader, PolygonMode polygonMode, PrimitveType drawType)
{
	initiallize();

	bindSSBO();

	update();

	m_vao->bind();
	auto face_index_num = m_Input->getFaceIndexArray()->size();
	auto line_index_num = m_Input->getLineIndexArray()->size();
	auto point_index_num = m_Input->getVertexIndexArray()->size();
	auto instance_num = m_Input->getInstancedArray()->getNumOfTuple();
	auto coordNum = m_Input->getVertextCoordArray()->getNumOfTuple();
	//instance_num=0;
	if (instance_num > 0) {
		shader->setBool("IsInstancedDraw", true);
	}
	else {
		shader->setBool("IsInstancedDraw", false);
	}

	auto hasMode = [&](PolygonMode mode)->bool {
		return (uint32_t)polygonMode & (uint32_t)mode;
		};



	if (hasMode(PolygonMode::line)) {
		shader->setPolygonMode((int)PolygonMode::line);
		//m_line_ebo->bind();
		if (instance_num > 0 && coordNum > 0)
			glDrawArraysInstanced((unsigned int)(PrimitveType::line_strip_adjacency), 0, coordNum+2, instance_num);

	}

	//if (hasMode(PolygonMode::point)) {
	//	//绑定对应的ebo
	//	//glEnable(GL_POLYGON_OFFSET_POINT);
	//	//glPolygonOffset(0.0f, -0.02f);
	//	glPointSize(5);
	//	shader->setPolygonMode((int)PolygonMode::point);
	//	m_point_ebo->bind();
	//	if (instance_num > 0 && point_index_num > 0)
	//		glDrawElementsInstanced((unsigned int)(PrimitveType::point), point_index_num, GL_UNSIGNED_INT, 0, instance_num);

	//}

	m_vao->release();
}

