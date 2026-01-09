#include "XGridItem.h"
#include <lib04_opengl/XOpenGLBuffer.h>
#include <lib01_shader/xshaderManger.h>
#include <lib04_opengl/XOpenGLVertexArrayObject.h>
#include "lib05_shape/actor2d/XTransformItem.h"

XGridItem::XGridItem(std::shared_ptr< XGraphicsItem> parent):XGraphicsItem(parent)
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

	m_transformItem = makeShareDbObject<XTransformItem>();
	addChildItem(m_transformItem);

	m_clipEnable = true;
}

XGridItem::~XGridItem()
{
}

void XGridItem::draw(const Eigen::Matrix4f& m)
{
	initiallize();
	auto shader = getShaderManger()->getGridShader2D();
	drawBorder(shader,m);

	auto selfTransform = this->getTransform();
	beginClip(m);

	Eigen::Matrix4f mat = m * getTransform().matrix();	//叠加父类的变换
	for (auto item : m_childItems) {
		item->draw(mat);
	}

	endClip();
}

void XGridItem::drawBorder(std::shared_ptr<xshader> border_shader, const Eigen::Matrix4f& m)
{
	initiallize();
	if(!getShaderManger() || !m_IsVisible)
		return;

	updateData();
	
	auto shader = getShaderManger()->getGridShader2D();
	shader->use();
	shader->setBool("isScreenGrid", m_isScreenGrid);

	shader->setFloat("gridSpace", m_gridSpace);
	shader->setInt("gridNum", m_gridNum);
	shader->setBool("showAxis", m_isShowAxis);
	//需要设置网格的原点
	shader->setVec2("Origin", 0, 0);

	Eigen::Matrix4f gridMat = getGridTransform().inverse().matrix();
	Eigen::Matrix4f parentMat = m* getTransform().matrix();
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

void XGridItem::setOrigin(const XQ::Vec2f& origin)
{
	mOrigin = origin;
	//m_gridTransform.translate(Eigen::Vector3f(-mOrigin.x, -mOrigin.y, 0));
	m_transformItem->translate(-mOrigin.x(), -mOrigin.y());
}

const XQ::Vec2f& XGridItem::getOrigin() const
{
	return mOrigin;
}

void XGridItem::gridReset()
{
	//m_gridTransform = Eigen::Affine3f::Identity();
	//m_gridTransform.translate(Eigen::Vector3f( - 1, -1, 0));

	m_transformItem->resetTransform();
	m_transformItem->translate(-1, -1);
}

void XGridItem::gridTranslate(float dx, float dy)
{
	//m_gridTransform.translate(Eigen::Vector3f( dx,dy,0));
	m_transformItem->translate(dx, dy);
}

void XGridItem::gridSale(float dx, float dy)
{
	//m_gridTransform.scale (Eigen::Vector3f( 1. / dx, 1. / dy,1));
	m_transformItem->scale(1. / dx, 1. / dy);
}

void XGridItem::gridSetSale(float sx, float sy)
{
	/*auto data = XQ::Matrix::transformDecomposition_TRS(m_gridTransform);
	data.sx = 2./sx;
	data.sy = 2./sy;
	m_gridTransform.matrix() = XQ::Matrix::computeMatrix(data);*/

	auto data = XQ::Matrix::transformDecomposition_TRS(m_transformItem->getTransform());
	data.sx = 2./sx;
	data.sy = 2./sy;
	m_transformItem->setTransform( XQ::Matrix::computeMatrix(data));
}

XQ::Vec2f XGridItem::gridGetSale() const
{
	/*auto data = XQ::Matrix::transformDecomposition_TRS(m_gridTransform);

	return XQ::Vec2f( 2./data.sx, 2./data.sy);*/

	auto data = XQ::Matrix::transformDecomposition_TRS(m_transformItem->getTransform());

	return XQ::Vec2f( 2./data.sx, 2./data.sy);
}

void XGridItem::setGridNum(int num)
{
	m_gridNum = num;
}

int XGridItem::getGridNum() const
{
	return m_gridNum;
}

void XGridItem::setGridSpace(double space)
{
	m_gridSpace = space;
}

double XGridItem::getGridSpace() const
{
	return m_gridSpace;
}

void XGridItem::setShowAxis(bool isShowaxis)
{
	m_isShowAxis = isShowaxis;
}

bool XGridItem::isShowAxis() const
{
	return m_isShowAxis;
}

void XGridItem::addLine(sptr<XGraphicsItem> line)
{
	m_transformItem->addChildItem(line);
}

Eigen::Affine3f XGridItem::getGridTransform() const
{
	//物体位姿 *M = 网格姿态；
	//return M;
	//auto t =m_gridTransform;
	//t.translate(Eigen::Vector3f(-mOrigin.x, -mOrigin.y, 0));
	//return t;
	return m_transformItem->getTransform();
}

uint32_t XGridItem::computeNumofVertices()
{
	if(m_drawType == PrimitveType::line_strip_adjacency)
		return m_coordArray->getNumOfTuple()+3;
	else
		return m_coordArray->getNumOfTuple();
}