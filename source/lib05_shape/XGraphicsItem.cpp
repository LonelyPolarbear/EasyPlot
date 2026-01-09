#include "XGraphicsItem.h"
#include <lib04_opengl/XOpenGLBuffer.h>
#include <lib04_opengl/XOpenGLVertexArrayObject.h>
#include <lib01_shader/xshader.h>
#include <lib02_camera/xcamera.h>

#include <glew/glew.h>
#include "xshape.h"

#include <lib00_utilty/XUtilty.h>
#include <lib04_opengl/XOpenGLEnable.h>
#include <lib01_shader/xshaderManger.h>
#include <iostream>

static std::atomic< uint64_t>  object_id_counter(0);

class XGraphicsItem::Internal {
public:
	Eigen::Affine3f m_transform = Eigen::Affine3f::Identity();
	std::mutex m_mutex;
	uint64_t m_id = ++object_id_counter;

	Eigen::Matrix4f m_sceneMat = Eigen::Matrix4f::Identity();
	bool isInitResource =false;
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
sptr<const XGraphicsItem> XGraphicsItem::getChildByID(int64_t id) const
{
	if (getID() == id) {
		return asDerived<XGraphicsItem>();
	}
	for (auto item : m_childItems) {
		if (item->getID() == id) {
			return item;
		}
		else {
			if(auto ret = item->getChildByID(id))
				return ret;
		}
	}
	return nullptr;
}
sptr<XGraphicsItem> XGraphicsItem::getChildByID(int64_t id)
{
	if (getID() == id) {
		return asDerived<XGraphicsItem>();
	}
	for (auto item : m_childItems) {
		if (item->getID() == id) {
			return item;
		}
		else {
			if (auto ret = item->getChildByID(id))
				return ret;
		}
	}
	return nullptr;
}
void XGraphicsItem::bindSSBO()
{
	m_ssbo_len->bind();
	m_ssbo_len->setBufferBindIdx(1);
}

void XGraphicsItem::draw(const Eigen::Matrix4f& m)
{
	initiallize();
	if (!m_IsVisible || !getShaderManger())
		return;

	auto glEnableObj = makeShareDbObject<XOpenGLEnable>();
	 {
		//!
		//! [1] 填充绘制
		if (m_IsFilled) {
			glEnableObj->disable(XOpenGLEnable::EnableType::BLEND);
			glEnableObj->disable(XOpenGLEnable::EnableType::DEPTH_TEST);
			drawFill(getShaderManger()->getFillShader(), m);
			glEnableObj->restore();
		}

		//!
		//! [2] 边框绘制
		glEnableObj->enable(XOpenGLEnable::EnableType::BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnableObj->disable(XOpenGLEnable::EnableType::DEPTH_TEST);
		drawBorder(getShaderManger()->getShader2D(getDrawType()), m);
		glEnableObj->restore();
	}

	//在绘制子类图元之前，可能会根据父图元的当前位置，调整子类图元位置
	updateChildPosition(m);

	Eigen::Matrix4f mat = m * d->m_transform.matrix();	//叠加父类的变换
	for (auto item : m_childItems) {
		item->draw(mat);
	}
}

void XGraphicsItem::pickBorderDraw(std::shared_ptr<xshader> shader,const Eigen::Matrix4f& m)
{
	initiallize();
	
	drawBorderImpl(shader, m, true);
	drawBorderImpl(shader, m, false);

	Eigen::Matrix4f mat = m * d->m_transform.matrix();	//叠加父类的变换
	for (auto item : m_childItems) {
		item->pickBorderDraw(/*getShaderManger()->getShader2D(getDrawType())*/shader,mat);
	}
}

void XGraphicsItem::pickFillDraw(std::shared_ptr<xshader> shader, const Eigen::Matrix4f& m)
{
	initiallize();

	drawFill(shader, m);

	Eigen::Matrix4f mat = m * d->m_transform.matrix();	//叠加父类的变换
	for (auto item : m_childItems) {
		item->pickFillDraw(/*getShaderManger()->getPickFillShader2D()*/shader, mat);
	}	
}

void XGraphicsItem::drawBorder(std::shared_ptr<xshader> shader,  const Eigen::Matrix4f& m ){
	initiallize();
	drawBorderImpl(shader, m, true);
	drawBorderImpl(shader, m, false);
}

void XGraphicsItem::drawBorderImpl(std::shared_ptr<xshader> shader, const Eigen::Matrix4f& m,bool isComputeLineLentgh)
{
	initiallize();
	if (!m_IsVisible /*|| isComposite()*/)
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
	shader->setPreSelectColor(m_preSelectColor.x(), m_preSelectColor.y(), m_preSelectColor.z(), m_preSelectColor.w());
	shader->setColorMode((int)m_colorMode);
	shader->setPolygonMode((int)m_polygonMode);
	shader->setSingleColor(m_singleColor.x(), m_singleColor.y(), m_singleColor.z(), m_singleColor.w());
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
	initiallize();
	if (!m_IsVisible /*|| isComposite()*/)
		return;
	updateData();
	shader->use();
	shader->setIsInstanceDarw(m_isInstance);
	shader->setObjectID(d->m_id);
	shader->setSingleColor(m_singleColor.x(), m_singleColor.y(), m_singleColor.z(), m_singleColor.w());
	shader->setFillColor(m_fillColor.x(), m_fillColor.y(), m_fillColor.z(), m_fillColor.w());
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
	initiallize();
	if(!m_clipEnable)
		return;
	auto shader = getShaderManger()->getFillShader();
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

void XGraphicsItem::setTransform(const Eigen::Matrix4f& m)
{
	d->m_transform.matrix() = m;
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
	m_vao->addBuffer(0, m_vbo_coord, 3, XOpenGL::DataType::float_, sizeof(XQ::Vec3f), 0);

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
			m_vao->addBuffer(1, m_instanceAttrBufffer, 4, XOpenGL::DataType::float_, sizeof(float) * 16, 0, true);
			m_vao->addBuffer(2, m_instanceAttrBufffer, 4, XOpenGL::DataType::float_, sizeof(float) * 16, sizeof(float) * 4, true);
			m_vao->addBuffer(3, m_instanceAttrBufffer, 4, XOpenGL::DataType::float_, sizeof(float) * 16, sizeof(float) * 8, true);
			m_vao->addBuffer(4, m_instanceAttrBufffer, 4, XOpenGL::DataType::float_, sizeof(float) * 16, sizeof(float) * 12, true);
		}

		m_instanceAttrBufffer->release();
	}

}

void XGraphicsItem::initiallize()
{
	if (d->isInitResource == false) {
		initResource();
		d->isInitResource = true;
	}
	return;
}

Eigen::Affine3f XGraphicsItem::getTransform() const {
	return d->m_transform;
}

Eigen::Affine3f XGraphicsItem::getParentAccumulateTransform() const
{
	Eigen::Affine3f transform = Eigen::Affine3f::Identity();
	auto parent = m_parentItem;
	while (parent)
	{
		transform = parent->getTransform() * transform;
		parent = parent->getParent();
	}
	
	return transform;
}

sptr<XGraphicsItem> XGraphicsItem::getParent() const
{
	return m_parentItem;
}

void XGraphicsItem::setParent(std::shared_ptr<XGraphicsItem> parent)
{
	m_parentItem = parent;
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

void XGraphicsItem::setSingleColor(const XQ::Vec4f& color)
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

void XGraphicsItem::setPreSelectColor(const XQ::Vec4f& color)
{
	m_preSelectColor = color;
}

void XGraphicsItem::setDrawType(PrimitveType type)
{
	m_drawType = type;
	}

//bool XGraphicsItem::isComposite() const
//{
//	return mIsComposite;
//}

XQ::Vec4f XGraphicsItem::getSingleColor() const
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
	//如果有子类，递归调用
	for (auto item : m_childItems) {
		item->setShaderManger(shaderManger);
	}
}

std::shared_ptr<xShaderManger> XGraphicsItem::getShaderManger() const
{
	if(m_shaderManger)
		return m_shaderManger;
	else{
		if (m_parentItem)
			return m_parentItem->getShaderManger();
		else
			return nullptr;
	}
}

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
	d->m_transform.rotate(Eigen::AngleAxisf(XQ::Matrix::radian(angle), Eigen::Vector3f::UnitZ()));
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
	setPosition(getPosition().x(), getPosition().y());
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

XQ::Vec2f XGraphicsItem::getPosition() const
{
	Eigen::Vector3f pos= d->m_transform.translation();
	return XQ::Vec2f(pos.x(), pos.y());
}

XQ::Vec2f XGraphicsItem::getPositionByOrientation() const
{
	if (m_positionType == XGL::PositionType::sceneScreen_center || m_positionType == XGL::PositionType::sceneScreen_complete) {
		if (m_orientation == XGL::Orientation::left_bottom) {
			return XQ::Vec2f(d->m_transform.translation().x(), d->m_transform.translation().y());
		}
		else if (m_orientation == XGL::Orientation::left_top) {
			return XQ::Vec2f(d->m_transform.translation().x(), -d->m_transform.translation().y());
		}
		else if (m_orientation == XGL::Orientation::right_bottom) {
			return XQ::Vec2f(-d->m_transform.translation().x(), d->m_transform.translation().y());
		}
		else if (m_orientation == XGL::Orientation::right_top) {
			return XQ::Vec2f(-d->m_transform.translation().x(), -d->m_transform.translation().y());
		}
	}
	
	return XQ::Vec2f(d->m_transform.translation().x(), d->m_transform.translation().y());
}

void XGraphicsItem::scale(float sx, float sy)
{
	d->m_transform.scale(Eigen::Vector3f(sx, sy, 1));
}

void XGraphicsItem::setScale(float sx, float sy)
{
	auto data = XQ::Matrix::transformDecomposition_TRS(d->m_transform);
	data.sx = sx;
	data.sy = sy;
	d->m_transform.matrix() = XQ::Matrix::computeMatrix(data);
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

void XGraphicsItem::setFillColor(const XQ::Vec4f& color)
{	
	m_fillColor = color;
}

XQ::Vec4f XGraphicsItem::getFillColor() const
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

bool XGraphicsItem::hitTest(const XQ::Vec2f& pos)
{
	return false;
}

XQ::BoundBox XGraphicsItem::getBoundBox()
{
	constexpr double limitMax = std::numeric_limits<double>::max();
	constexpr double limitMin = std::numeric_limits<double>::lowest();;
	XQ::Vec3d minBound = XQ::Vec3d(limitMax, limitMax, limitMax);
	XQ::Vec3d maxBound = XQ::Vec3d(limitMin, limitMin, limitMin);

	if (m_coordArray->size() == 0)
		return XQ::BoundBox{ minBound.x(), minBound.y(), minBound.z(), maxBound.x(), maxBound.y(), maxBound.z()};

	float* pData = m_coordArray->data(0);
	for (int i = 0; i < m_coordArray->getNumOfTuple(); i++) {

		Eigen::Vector3f pos = Eigen::Vector3f(pData[i * 3 + 0], pData[i * 3 + 1], pData[i * 3 + 2]);
		pos = d->m_transform * pos;

		minBound = XQ::Vec3d(std::min<double>(minBound.x(), pos.x()), std::min<double>(minBound.y(), pos.y()), pData[i * 3 + 2]);
		maxBound = XQ::Vec3d(std::max<double>(maxBound.x(), pos.x()), std::max<double>(maxBound.y(), pos.y()), pData[i * 3 + 2]);
	}
	return XQ::BoundBox{ minBound.x(), minBound.y(), minBound.z(), maxBound.x(), maxBound.y(), maxBound.z()};
}

float* XGraphicsItem::getMatrix() const
{
	return d->m_transform.matrix().data();
}

void XGraphicsItem::updateData()
{
	std::lock_guard<std::mutex> lock(d->m_mutex);
	//当数据更新时，需要更新所有的buffer
	updateVboCoord();

	updateVboColor();

	updateVboEbo();

	updateVboInstance();

	//数据已更新，刷新时间戳
	m_UpdateTime.Modified();
}

void XGraphicsItem::updateVboCoord()
{
	//顶点数据已经更新
	auto m_coord = m_coordArray;
	if (m_coord && m_coord->GetTimeStamp() > m_UpdateTime) {

		m_vbo_coord->bind();
		m_vbo_coord->allocate(m_coord->data(0), m_coord->size());
		m_vbo_coord->release();
	}
}

void XGraphicsItem::updateVboColor()
{
	////顶点颜色数据已经更新
	auto m_VertexColor = m_colorArray;
	if (m_VertexColor && m_VertexColor->GetTimeStamp() > m_UpdateTime) {
		m_vbo_color->bind();

		m_vbo_color->allocate(m_VertexColor->data(0), m_VertexColor->size());

		m_vbo_color->release();
	}
}

void XGraphicsItem::updateVboEbo()
{
	auto m_index = m_indexArray;
	if (m_index && m_index->GetTimeStamp() > m_UpdateTime) {

		m_ebo->bind();
		m_ebo->allocate(m_index->data(0), m_index->size());
	}
}

void XGraphicsItem::updateVboInstance()
{
	if (m_instacePos && m_instacePos->GetTimeStamp() > m_UpdateTime) {

		m_instanceAttrBufffer->bind();

		m_instanceAttrBufffer->allocate(m_instacePos->data(0), m_instacePos->size());

		m_instanceAttrBufffer->release();
	}
}

void XGraphicsItem::setSceneMatrix(const Eigen::Matrix4f& m)
{
	d->m_sceneMat = m;
	for (auto item : m_childItems) {
		item->setSceneMatrix(m);
	}
}

Eigen::Matrix4f XGraphicsItem::getSceneMatrix() const
{
	return d->m_sceneMat;
}


uint32_t XGraphicsItem::computeNumofVertices() {
	return m_coordArray->getNumOfTuple();
 }

//void XGraphicsItem::setIsComposite(bool enable)
//{
//	mIsComposite = enable;
//}

void XGraphicsItem::addChildItem(std::shared_ptr<XGraphicsItem> item)
{
	if (item) {
		item->setShaderManger(getShaderManger());
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

bool XGraphicsItem::addAttribute(const std::wstring& key, const std::wstring& value)
{
	if(key.empty() || value.empty())
		return false;
	if (m_attributes.find(key) != m_attributes.end()) {
		return false;
	}
	m_attributes[key] = value;
	return true;
}

bool XGraphicsItem::removeAttribute(const std::wstring& key)
{
	if (m_attributes.find(key) != m_attributes.end()) {
		m_attributes.erase(key);
		return true;
	}
	return false;
}

std::wstring XGraphicsItem::getAttribute(const std::wstring& key) const
{
	auto it = m_attributes.find(key);
	if (it != m_attributes.end()) {
		return it->second;
	}
	return std::wstring();
}

std::map<std::wstring, std::wstring> XGraphicsItem::getAttributes() const
{
	return m_attributes;
}

LocalCoordCompute::LocalCoordCompute(const Eigen::Matrix4f& transform)
{
	m_transform.matrix() = transform;
	leftBot = m_transform * Eigen::Vector3f(-1, -1, 0);
	rightBot = m_transform * Eigen::Vector3f(1, -1, 0);
	leftTop = m_transform * Eigen::Vector3f(-1, 1, 0);
	rightTop = m_transform * Eigen::Vector3f(1, 1, 0);
	center = m_transform * Eigen::Vector3f(0, 0, 0);

#if 0
	dir_x = (rightBot - leftBot).normalized();
	dir_y = (leftTop - leftBot).normalized();
	scalex = (rightBot - leftBot).norm() / 2;
	scaley = (leftTop - leftBot).norm() / 2;
	{
		Eigen::Vector3f dir_x = m_transform.linear() * Eigen::Vector3f::UnitX();
		Eigen::Vector3f dir_y = m_transform.linear() * Eigen::Vector3f::UnitY();
		auto scalex = dir_x.norm();
		auto scaley = dir_y.norm();
	}
#else
	dir_x = m_transform.linear() * Eigen::Vector3f::UnitX();
	dir_y = m_transform.linear() * Eigen::Vector3f::UnitY();
	scalex = dir_x.norm();
	scaley = dir_y.norm();
	dir_x.normalize();
	dir_y.normalize();
#endif
}