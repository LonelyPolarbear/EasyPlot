#include "XGraphicsItem.h"
#include <lib04_opengl/XOpenGLBuffer.h>
#include <lib04_opengl/XOpenGLVertexArrayObject.h>
#include <lib01_shader/xshader.h>
#include <lib02_camera/xcamera.h>

#include <glew/glew.h>
#include "xshape.h"

#include <lib00_utilty/myUtilty.h>
#include <lib04_opengl/XOpenGLEnable.h>
#include <lib01_shader/xshaderManger.h>
#include <iostream>

static std::atomic< uint64_t>  object_id_counter(0);

class XGraphicsItem::Internal {
public:
	Eigen::Affine3f m_transform = Eigen::Affine3f::Identity();
	//Eigen::Affine3f m_loc2gridTrans = Eigen::Affine3f::Identity();		//局部到网格坐标系的变换，表达的是网格坐标系在局部坐标系的位姿
	std::mutex m_mutex;
	uint64_t m_id = ++object_id_counter;
};


XGraphicsItem::XGraphicsItem(std::shared_ptr<XGraphicsItem> parent):d(new Internal()),m_parentItem(parent)
{
	m_vao = makeShareDbObject<XOpenGLVertexArrayObject>();
	m_vbo_coord = makeShareDbObject<XOpenGLBuffer>();
	m_ebo = makeShareDbObject<XOpenGLBuffer>();
	m_ssbo_len = makeShareDbObject<XOpenGLBuffer>();
	m_instanceAttrBufffer = makeShareDbObject<XOpenGLBuffer>();
}

XGraphicsItem::~XGraphicsItem()
{
}

int64_t XGraphicsItem::getID() const {
	return d->m_id;
}
void XGraphicsItem::bindSSBO()
{
	m_ssbo_len->bind();
	m_ssbo_len->setBufferBindIdx(1);
}

void XGraphicsItem::draw(const Eigen::Matrix4f& m)
{
	if (!m_IsVisible || !m_shaderManger)
		return;

	auto glEnableObj = makeShareDbObject<XOpenGLEnable>();

	//!
	//! [1] 填充绘制
	if (m_IsFilled) {
		glEnableObj->disable(XOpenGLEnable::EnableType::BLEND);
		glEnableObj->disable(XOpenGLEnable::EnableType::DEPTH_TEST);
		drawFill(m_shaderManger->getFillShader(), m);
		glEnableObj->restore();
	}
	
	//!
	//! [2] 边框绘制
	glEnableObj->enable(XOpenGLEnable::EnableType::BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnableObj->disable(XOpenGLEnable::EnableType::DEPTH_TEST);
	drawBorder(m_shaderManger->getShader2D(getDrawType()),m);
	glEnableObj->restore();

	//!
	//! [3] 绘制子类图元
	
	//beginClip(m);			//需要知道当前图元的层级，

	Eigen::Matrix4f mat = m * d->m_transform.matrix();	//叠加父类的变换
	for (auto item : m_childItems) {
		item->draw(mat);
	}

	//endClip();
}

void XGraphicsItem::pickBorderDraw(std::shared_ptr<xshader> shader,const Eigen::Matrix4f& m)
{
	drawBorderImpl(shader, m, true);

	Eigen::Matrix4f mat = m * d->m_transform.matrix();	//叠加父类的变换
	for (auto item : m_childItems) {
		item->drawBorderImpl(m_shaderManger->getShader2D(getDrawType()),mat,true);
	}
}

void XGraphicsItem::pickFillDraw(std::shared_ptr<xshader> shader, const Eigen::Matrix4f& m)
{
	Eigen::Matrix4f mat = m * d->m_transform.matrix();	//叠加父类的变换
	for (auto item : m_childItems) {
		item->drawFill(m_shaderManger->getPickFillShader2D(), mat);
	}

	drawFill(shader, m);
}

void XGraphicsItem::drawBorder(std::shared_ptr<xshader> shader,  const Eigen::Matrix4f& m ){
	drawBorderImpl(shader, m, true);
	drawBorderImpl(shader, m, false);
}

void XGraphicsItem::drawBorderImpl(std::shared_ptr<xshader> shader, const Eigen::Matrix4f& m,bool isComputeLineLentgh)
{
	if (!m_IsVisible)
		return;
	updateData();
	shader->use();
	shader->setIsInstanceDarw(m_isInstance);
	shader->setBool("isComputeLineLentgh", isComputeLineLentgh);
	shader->setObjectID(d->m_id);
	shader->setLineWidth(m_lineWidth);
	shader->setPenStyle((int)getPenStyle());
	shader->setPositionType((int)getPositionType());
	shader->setOrientation((int)getOrientation());
	shader->setConenctSmoothEnable(m_connectSmoothEnable);
	shader->setPreSelectColor(m_preSelectColor.x, m_preSelectColor.y, m_preSelectColor.z, m_preSelectColor.w);
	shader->setColorMode((int)m_colorMode);
	shader->setPolygonMode((int)m_polygonMode);
	shader->setSingleColor(m_singleColor.x, m_singleColor.y, m_singleColor.z, m_singleColor.w);
	shader->setfixedLine(m_IsFixedLine);

	bindSSBO();

	Eigen::Matrix4f mat = m;
	Eigen::Matrix4f selfMat = d->m_transform.matrix();

	shader->setModelMatrix(mat.data());
	shader->setModelSelfMatrix(selfMat.data());

	m_vao->bind();
	if (m_polygonMode == PolygonMode::line) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		auto num = computeNumofVertices();
		glDrawArrays((unsigned int)m_drawType, 0, num);
	}

	if (m_polygonMode == PolygonMode::point) {
		glPolygonOffset(1.0f, 1.0f);
		glDrawArrays(GL_POINTS, 0, m_coordArray->getNumOfTuple());
	}

	shader->unUse();
	m_vao->release();
}

void XGraphicsItem::drawFill(std::shared_ptr<xshader> shader, const Eigen::Matrix4f& m)
{
	if (!m_IsVisible)
		return;
	updateData();
	shader->use();
	shader->setIsInstanceDarw(m_isInstance);
	shader->setObjectID(d->m_id);
	shader->setSingleColor(m_singleColor.x, m_singleColor.y, m_singleColor.z, m_singleColor.w);
	shader->setFillColor(m_fillColor.x, m_fillColor.y, m_fillColor.z, m_fillColor.w);
	shader->setPositionType((int)getPositionType());
	shader->setOrientation((int)getOrientation());

	//Eigen::Matrix4f mat = m * d->m_transform.matrix();
	//Eigen::Matrix4f selfMat = Eigen::Matrix4f::Identity();
	//shader->setModelMatrix(mat.data());
	//shader->setModelSelfMatrix(selfMat.data());

	Eigen::Matrix4f mat = m;
	Eigen::Matrix4f selfMat = d->m_transform.matrix();
	shader->setModelMatrix(mat.data());
	shader->setModelSelfMatrix(selfMat.data());

	m_vao->bind();

	//1.如果是封闭图形，且开启了填充
	//2.如果是封闭图形，且开启了剪切
	if (  (m_IsFilled && m_IsClosed)  || (m_clipEnable && m_IsClosed)) {
		if (m_indexArray) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			auto indexCount = m_indexArray->getNumOfTuple() * m_indexArray->getComponent();
			if (m_isInstance ) {
				if(m_instacePos){
					auto instanceNum =  m_instacePos->getNumOfTuple();
					if(instanceNum > 0)
						glDrawElementsInstanced((unsigned int)PrimitveType::triangle, indexCount, GL_UNSIGNED_INT, (void*)0, instanceNum);  // 最后一个参数是实例数量
				}
				else {
					//std::cout<<__FILE__ <<" Line:" <<std::dec <<__LINE__ << " m_instacePos is null" << std::endl;
				}
			}
			else {
				glDrawElements((unsigned int)PrimitveType::triangle, indexCount, GL_UNSIGNED_INT, 0);
			}
		}
	}

	shader->unUse();
	m_vao->release();
}

void XGraphicsItem::beginClip(const Eigen::Matrix4f& m)
{
	if(!m_clipEnable)
		return;
	auto shader = m_shaderManger->getFillShader();
	shader->use();
	//在绘制之前，需要利用模板缓冲，避免图表外的曲线点被渲染
	//开启模板测试
	glEnable(GL_STENCIL_TEST);
	//glClearStencil(0);
	//glClear(GL_STENCIL_BUFFER_BIT);
	// 3. 只写入模板缓冲区，不写入颜色
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	// 4. 绘制模板缓冲区
	//auto mat = m * d->m_transform.matrix();
	drawFill(shader, m);

	// 5. 恢复颜色写入，设置模板测试
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glStencilFunc(GL_EQUAL, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

	shader->unUse();
}

void XGraphicsItem::endClip()
{
	if (!m_clipEnable)
		return;
	glDisable(GL_STENCIL_TEST);
}

void XGraphicsItem::initResource()
{
	m_vao->create();
	m_vao->bind();

	m_vbo_coord->setBufferType(XOpenGLBuffer::VertexBuffer);

	m_vbo_coord->setUsagePattern(XOpenGLBuffer::StaticDraw);

	m_vbo_coord->create();

	m_vbo_coord->bind();

	m_ssbo_len->setBufferType(XOpenGLBuffer::ShaderStorageBuffer);

	m_ssbo_len->setUsagePattern(XOpenGLBuffer::StaticDraw);

	m_ssbo_len->create();

	m_ssbo_len->bind();

	m_ebo->setBufferType(XOpenGLBuffer::IndexBuffer);

	m_ebo->setUsagePattern(XOpenGLBuffer::StaticDraw);

	m_ebo->create();

	m_ebo->bind();			//用于填充时候绘制，填充目前只支持三角形填充

	//设置顶点属性
	m_vao->addBuffer(0, m_vbo_coord, 3, (unsigned int)XOpenGLValueType::Float32, sizeof(myUtilty::Vec3f), 0);

	m_vbo_coord->release();

	m_vao->release();
	//return;
	{
		m_instanceAttrBufffer->setBufferType(XOpenGLBuffer::VertexBuffer);

		m_instanceAttrBufffer->setUsagePattern(XOpenGLBuffer::StaticDraw);

		m_instanceAttrBufffer->create();

		m_instanceAttrBufffer->bind();

		//设置顶点属性 每个顶点对应一个矩形 对应一个变换矩阵
		if (m_isInstance) {
			// AI：如果VBO最后没有分配内存，则不应该设置顶点属性，即使该属性在着色器中没有使用到
			m_vao->addBuffer(1, m_instanceAttrBufffer, 4, (unsigned int)XOpenGLValueType::Float32, sizeof(float) * 16, 0, true);
			m_vao->addBuffer(2, m_instanceAttrBufffer, 4, (unsigned int)XOpenGLValueType::Float32, sizeof(float) * 16, sizeof(float) * 4, true);
			m_vao->addBuffer(3, m_instanceAttrBufffer, 4, (unsigned int)XOpenGLValueType::Float32, sizeof(float) * 16, sizeof(float) * 8, true);
			m_vao->addBuffer(4, m_instanceAttrBufffer, 4, (unsigned int)XOpenGLValueType::Float32, sizeof(float) * 16, sizeof(float) * 12, true);
		}

		m_instanceAttrBufffer->release();
	}

}

Eigen::Affine3f XGraphicsItem::getTransform() const {
	return d->m_transform;
}

void XGraphicsItem::setCoordArray(std::shared_ptr<XFloatArray> coordArray)
{
	m_coordArray = coordArray;
}

void XGraphicsItem::setIndexArray(std::shared_ptr<XUIntArray> coordArray)
{
	m_indexArray = coordArray;
}

void XGraphicsItem::setColorArray(std::shared_ptr<XFloatArray> colorArray)
{
	m_colorArray = colorArray;
}

void XGraphicsItem::setInstanceArray(std::shared_ptr<XFloatArray> instanceArray)
{
	m_instacePos = instanceArray;
}

void XGraphicsItem::setSingleColor(const myUtilty::Vec4f& color)
{
	m_singleColor = color;
}

void XGraphicsItem::setPolygonMode(PolygonMode mode)
{
	m_polygonMode = mode;
}

void XGraphicsItem::setColorMode(ColorMode mode)
{
	m_colorMode = mode;
}

void XGraphicsItem::setPreSelectColor(const myUtilty::Vec4f& color)
{
	m_preSelectColor = color;
}

void XGraphicsItem::setDrawType(PrimitveType type)
{
	m_drawType = type;
	}

myUtilty::Vec4f XGraphicsItem::getSingleColor() const
{
	return m_singleColor;
}

PrimitveType XGraphicsItem::getDrawType() const
{
	return m_drawType;
}

void XGraphicsItem::setLineWidth(uint32_t width)
{
	m_lineWidth = width;
}

void XGraphicsItem::setShaderManger(std::shared_ptr<xShaderManger> shaderManger)
{
	m_shaderManger = shaderManger;
}

std::shared_ptr<xShaderManger> XGraphicsItem::getShaderManger() const
{
	return m_shaderManger;
}

#if 0
void XGraphicsItem::gridReset()
{
	d->m_loc2gridTrans.setIdentity();
}

void XGraphicsItem::gridTranslate(float dx, float dy)
{
	d->m_loc2gridTrans.translate(Eigen::Vector3f(dx, dy, 0));
}

void XGraphicsItem::gridSale(float dx, float dy)
{
	d->m_loc2gridTrans.scale(Eigen::Vector3f(1.0f / dx, 1.0f / dy, 1));
}

Eigen::Affine3f XGraphicsItem::getGridTransform() const
{
	return d->m_loc2gridTrans;
}
#endif

uint32_t XGraphicsItem::getLineWidth() const
{
	return m_lineWidth;
}

std::shared_ptr<XFloatArray> XGraphicsItem::getCoordArray() const
{
	return m_coordArray;
}

void XGraphicsItem::rotate(float angle)
{
	d->m_transform.rotate(Eigen::AngleAxisf(myUtilty::Matrix::radian(angle), Eigen::Vector3f::UnitZ()));
}

void XGraphicsItem::translate(float dx, float dy)
{
	d->m_transform.translate(Eigen::Vector3f(dx, dy, 0));
}

void XGraphicsItem::setPosition(float x, float y)
{
	if (m_positionType == XGL::PositionType::sceneScreen_center || m_positionType == XGL::PositionType::sceneScreen_complete) {
		if (m_orientation == XGL::Orientation::left_bottom) {
			d->m_transform.translation() << x, y, 0;
		}else if (m_orientation == XGL::Orientation::left_top) {
			d->m_transform.translation() << x, -y, 0;
		}else if (m_orientation == XGL::Orientation::right_bottom) {
			d->m_transform.translation() << -x, y, 0;
		}else if (m_orientation == XGL::Orientation::right_top) {
			d->m_transform.translation() << -x, -y, 0;	
		}
	}else
		d->m_transform.translation()<<x,y,0;
}

void XGraphicsItem::setPositionType(XGL::PositionType type)
{
	m_positionType = type;
	setPosition(getPosition().x, getPosition().y);
}

XGL::PositionType XGraphicsItem::getPositionType() const
{
	return m_positionType;
}

void XGraphicsItem::setOrientation(XGL::Orientation orientation)
{
	//auto old_orientation = m_orientation;
	//auto pos = getPositionByOrientation();
	m_orientation = orientation;
	//setPosition(pos.x, pos.y);
}

XGL::Orientation XGraphicsItem::getOrientation() const
{
	return m_orientation;
}

myUtilty::Vec2f XGraphicsItem::getPosition() const
{
	Eigen::Vector3f pos= d->m_transform.translation();
	return myUtilty::Vec2f(pos.x(), pos.y());
}

myUtilty::Vec2f XGraphicsItem::getPositionByOrientation() const
{
	if (m_positionType == XGL::PositionType::sceneScreen_center || m_positionType == XGL::PositionType::sceneScreen_complete) {
		if (m_orientation == XGL::Orientation::left_bottom) {
			return myUtilty::Vec2f(d->m_transform.translation().x(), d->m_transform.translation().y());
		}
		else if (m_orientation == XGL::Orientation::left_top) {
			return myUtilty::Vec2f(d->m_transform.translation().x(), -d->m_transform.translation().y());
		}
		else if (m_orientation == XGL::Orientation::right_bottom) {
			return myUtilty::Vec2f(-d->m_transform.translation().x(), d->m_transform.translation().y());
		}
		else if (m_orientation == XGL::Orientation::right_top) {
			return myUtilty::Vec2f(-d->m_transform.translation().x(), -d->m_transform.translation().y());
		}
	}
	
	return myUtilty::Vec2f(d->m_transform.translation().x(), d->m_transform.translation().y());
}

void XGraphicsItem::scale(float sx, float sy)
{
	d->m_transform.scale(Eigen::Vector3f(sx, sy, 1));
}

void XGraphicsItem::resetTransform()
{
	d->m_transform.setIdentity();
}

void XGraphicsItem::setVisible(bool visible)
{
	m_IsVisible = visible;
}

bool XGraphicsItem::isVisible()
{
	return m_IsVisible;
}

void XGraphicsItem::setFixedLine(bool fixed)
{
	m_IsFixedLine = fixed;
}

void XGraphicsItem::setPenStyle(PenStyle style)
{
	m_penStyle = style;
}

void XGraphicsItem::setConnectSmoothEnable(bool enable)
{
	m_connectSmoothEnable = enable;
}

bool XGraphicsItem::isConnectSmoothEnable() const
{
	return m_connectSmoothEnable;
}

XGraphicsItem::PenStyle XGraphicsItem::getPenStyle() const
{
	return m_penStyle;
}

void XGraphicsItem::setFillColor(const myUtilty::Vec4f& color)
{	
	m_fillColor = color;
}

myUtilty::Vec4f XGraphicsItem::getFillColor() const
{
	return m_fillColor;
}

void XGraphicsItem::setIsFilled(bool filled)
{
	m_IsFilled = filled;
}

bool XGraphicsItem::isFilled() const
{
	return m_IsFilled;
}

bool XGraphicsItem::hitTest(const myUtilty::Vec2f& pos)
{
	return false;
}

myUtilty::BoundBox XGraphicsItem::getBoundBox()
{
	constexpr double limitMax = std::numeric_limits<double>::max();
	constexpr double limitMin = std::numeric_limits<double>::lowest();;
	myUtilty::Vec3d minBound = myUtilty::Vec3d(limitMax, limitMax, limitMax);
	myUtilty::Vec3d maxBound = myUtilty::Vec3d(limitMin, limitMin, limitMin);

	if (m_coordArray->size() == 0)
		return myUtilty::BoundBox{ minBound.x, minBound.y, minBound.z, maxBound.x, maxBound.y, maxBound.z };

	float* pData = m_coordArray->data(0);
	for (int i = 0; i < m_coordArray->getNumOfTuple(); i++) {

		Eigen::Vector3f pos = Eigen::Vector3f(pData[i * 3 + 0], pData[i * 3 + 1], pData[i * 3 + 2]);
		pos = d->m_transform * pos;

		minBound = myUtilty::Vec3d(std::min<double>(minBound.x, pos.x()), std::min<double>(minBound.y, pos.y()), pData[i * 3 + 2]);
		maxBound = myUtilty::Vec3d(std::max<double>(maxBound.x, pos.x()), std::max<double>(maxBound.y, pos.y()), pData[i * 3 + 2]);
	}
	return myUtilty::BoundBox{ minBound.x, minBound.y, minBound.z, maxBound.x, maxBound.y, maxBound.z };
}

float* XGraphicsItem::getMatrix() const
{
	return d->m_transform.matrix().data();
}

void XGraphicsItem::updateData()
{
	std::lock_guard<std::mutex> lock(d->m_mutex);
	
	//顶点数据已经更新
	auto m_coord =m_coordArray;
	if (m_coord && m_coord->GetTimeStamp() > m_UpdateTime) {

		m_vbo_coord->bind();
		m_vbo_coord->allocate(m_coord->data(0), m_coord->size());
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

uint32_t XGraphicsItem::computeNumofVertices() {
	return m_coordArray->getNumOfTuple();
 }

void XGraphicsItem::addChildItem(std::shared_ptr<XGraphicsItem> item)
{
	if (item) {
		if (!item->getShaderManger()) {
			item->setShaderManger(m_shaderManger);
		}
		m_childItems.push_back(item);
	}
}

void XGraphicsItem::addChildItem(std::vector<std::shared_ptr<XGraphicsItem>> items)
{
	for(auto item : items)
		addChildItem(item);
}

void XGraphicsItem::removeChildItem(std::shared_ptr<XGraphicsItem> item)
{
	if (item) {
		auto it = std::find(m_childItems.begin(), m_childItems.end(), item);
		if (it != m_childItems.end()) {
			m_childItems.erase(it);
		}
	}
}

void XGraphicsItem::clearChildItems()
{
	m_childItems.clear();
}

void XGraphicsItem::setParentItem(std::shared_ptr<XGraphicsItem> item)
{
	m_parentItem = item;
}

std::shared_ptr<XGraphicsItem> XGraphicsItem::getParentItem() const
{
	return m_parentItem;
}
