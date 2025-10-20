#include "XGridItem.h"
#include <lib04_opengl/XOpenGLBuffer.h>
#include <lib01_shader/xshaderManger.h>
#include <lib04_opengl/XOpenGLVertexArrayObject.h>

XGridItem::XGridItem():XGraphicsItem()
{
	this->setDrawType(PrimitveType::line_strip_adjacency);
	auto coord = makeShareDbObject<XFloatArray>();

	coord->setComponent(3);
	coord->setNumOfTuple(4);

	coord->setTuple(0, -1, -1, 0);
	coord->setTuple(1, 1, -1, 0);
	coord->setTuple(2, 1, 1, 0);
	coord->setTuple(3, -1, 1, 0);

	this->setCoordArray(coord);

	auto index = makeShareDbObject<XUIntArray>();
	index->setComponent(3);
	index->setNumOfTuple(2);
	index->setTuple(0, 0, 1, 2);
	index->setTuple(1, 0, 2, 3);

	this->setIndexArray(index);
}

XGridItem::~XGridItem()
{
}

void XGridItem::draw(const Eigen::Matrix4f& m)
{
	auto shader = m_shaderManger->getGridShader2D();
	drawBorder(shader,m);
}

void XGridItem::drawBorder(std::shared_ptr<xshader> border_shader, const Eigen::Matrix4f& m)
{
	if(!m_shaderManger || !m_IsVisible)
		return;

	updateData();
	
	auto shader = m_shaderManger->getGridShader2D();
	shader->use();
	shader->setBool("isScreenGrid", m_isScreenGrid);
	//需要设置网格的原点
	shader->setVec2("Origin",mOrigin.x,mOrigin.y);

	Eigen::Matrix4f gridMat = getTransform().matrix().inverse();
	Eigen::Matrix4f parentMat = m;
	shader->setModelMatrix(parentMat.data());
	shader->setMat4("ObjectMat", gridMat.data());

	m_vao->bind();


	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawElements((unsigned int)PrimitveType::triangle, m_indexArray->getNumOfTuple() * m_indexArray->getComponent(), GL_UNSIGNED_INT, 0);


	shader->unUse();
	m_vao->release();
}

void XGridItem::setIsScreenGrid(bool isScreenGrid)
{
	m_isScreenGrid = isScreenGrid;
}

bool XGridItem::isScreenGrid() const
{
	return m_isScreenGrid;
}

void XGridItem::setOrigin(const myUtilty::Vec2f& origin)
{
	mOrigin = origin;
}

const myUtilty::Vec2f& XGridItem::getOrigin() const
{
	return mOrigin;
}

void XGridItem::updateData()
{
	//std::lock_guard<std::mutex> lock(d->m_mutex);

	//顶点数据已经更新
	auto m_coord = m_coordArray;
	if (m_coord && m_coord->GetTimeStamp() > m_UpdateTime) {

		m_vbo_coord->bind();


		m_vbo_coord->allocate(m_coord->data(0), m_coord->size());
		m_indexArray->setTuple(0, 0, 1, 2);
		m_indexArray->setTuple(1, 0, 2,3);
		m_indexArray->Modified();

		m_vbo_coord->release();
	}

	auto m_index = m_indexArray;
	if (m_index && m_index->GetTimeStamp() > m_UpdateTime) {

		m_ebo->bind();
		m_ebo->allocate(m_index->data(0), m_index->size());
		//m_vbo_coord->release();
	}

	////顶点颜色数据已经更新
	auto m_VertexColor = m_colorArray;
	if (m_VertexColor && m_VertexColor->GetTimeStamp() > m_UpdateTime) {
		m_vbo_color->bind();

		m_vbo_color->allocate(m_VertexColor->data(0), m_VertexColor->size());

		m_vbo_color->release();
	}

	//数据已更新，刷新时间戳
	m_UpdateTime.Modified();
}

uint32_t XGridItem::computeNumofVertices()
{
	if(m_drawType == PrimitveType::line_strip_adjacency)
		return m_coordArray->getNumOfTuple()+3;
	else
		return m_coordArray->getNumOfTuple();
}
