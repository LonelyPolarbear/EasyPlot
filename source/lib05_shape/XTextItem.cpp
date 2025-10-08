#include "XTextItem.h"
#include <lib04_opengl/XOpenGLBuffer.h>
#include <lib04_opengl/XOpenGLEnable.h>
#include <lib04_opengl/XOpenGLVertexArrayObject.h>
#include <lib01_shader/xshaderManger.h>
#include <lib08_freetype/xfreetype.h>
#include <Eigen/Eigen>

XTextItem::XTextItem():XGraphicsItem()
{
	m_textureBuffer = makeShareDbObject<XOpenGLBuffer>();

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
	index->setTuple(0, 1, 2, 3);
	index->setTuple(1, 1, 3, 4);

	this->setIndexArray(index);

	this->setIsFilled(true);

	m_isInstance = true;

	m_textureArray = makeShareDbObject<XFloatArray>();
	m_textureArray->setComponent(3);
}

XTextItem::~XTextItem()
{
}

void XTextItem::draw(const Eigen::Matrix4f& m)
{
	if (!m_IsVisible || !m_shaderManger)
		return;

	auto glEnableObj = makeShareDbObject<XOpenGLEnable>();

	//!
	//! [1] 填充绘制
	if (m_IsFilled) {
		glEnableObj->enable(XOpenGLEnable::EnableType::BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnableObj->disable(XOpenGLEnable::EnableType::DEPTH_TEST);
		drawFill(m_shaderManger->getTextShader(), m);
		glEnableObj->restore();
	}
}

void XTextItem::initResource()
{
	XGraphicsItem::initResource();

	//设置纹理属性
	m_vao->bind();

	m_textureBuffer->setBufferType(XOpenGLBuffer::VertexBuffer);

	m_textureBuffer->setUsagePattern(XOpenGLBuffer::StaticDraw);

	m_textureBuffer->create();

	m_vao->addBuffer(5, m_textureBuffer, 3, (unsigned int)XOpenGLValueType::Float32, sizeof(float) * 12, sizeof(float) * 3*0, true);
	m_vao->addBuffer(6, m_textureBuffer, 3, (unsigned int)XOpenGLValueType::Float32, sizeof(float) * 12, sizeof(float) * 3 * 1, true);
	m_vao->addBuffer(7, m_textureBuffer, 3, (unsigned int)XOpenGLValueType::Float32, sizeof(float) * 12, sizeof(float) * 3 * 2, true);
	m_vao->addBuffer(8, m_textureBuffer, 3, (unsigned int)XOpenGLValueType::Float32, sizeof(float) * 12, sizeof(float) * 3 * 3, true);

	m_vao->release();
	
}

void XTextItem::setText(const std::wstring& text)
{
	m_text = text;
	//设置实例化属性
	m_instacePos = makeShareDbObject<XFloatArray>();
	m_instacePos->setComponent(16);
	

	//遍历text，确认有效字符的个数
	auto num = text.size();
	int validNum = 0;
	for (int i = 0; i < num; i++) {
		auto c = text.at(i);
		if (c != '\n') {
			validNum++;
		}
	}
	m_instacePos->setNumOfTuple(validNum);
	m_textureArray->setNumOfTuple(validNum *4);
	//获取每个字符的纹理
	int start_x = 0;
	int start_y = 0;

	double scale = (double)m_fontSize/(double)64;		//字形的缩放系数
	int idx = 0;
	for (int i=0;i< num;i++) {
		auto c = text.at(i);
		if (c == '\n') {
			start_x = 0;
			start_y -= 64;
			continue;
		}
		else {
			auto glyph = xfreetype::Instance()->getCharacterSdf(c);
			auto layer = glyph.layer;
			auto width = glyph.width;
			auto height = glyph.height;
			auto x = glyph.x * xfreetype::Instance()->getSdfSingleTextWidth();		//字形位置
			auto y = glyph.y * xfreetype::Instance()->getSdfSingleTextHeight();
			y += (xfreetype::Instance()->getSdfSingleTextHeight() - height);
			auto picture_width = xfreetype::Instance()->getSdfPictureWidth();
			auto fontBlockWidth = xfreetype::Instance()->getSdfSingleTextWidth();
			auto x_ = (float)x / picture_width;
			auto y_ = (float)y / picture_width;
			auto w_ = (float)fontBlockWidth / picture_width;
			auto h_ = (float)fontBlockWidth / picture_width;

			m_textureArray->setTuple(4 * idx + 0, x_, y_, layer);
			m_textureArray->setTuple(4 * idx + 1, x_ + w_, y_, layer);
			m_textureArray->setTuple(4 * idx + 2, x_ + w_, y_ + h_, layer);
			m_textureArray->setTuple(4 * idx + 3, x_, y_ + h_, layer);

			//字符位置

			float scale_x = glyph.width * 0.5*scale;
			float scale_y = glyph.height * 0.5 * scale;
			
			Eigen::Affine3f tranform = Eigen::Affine3f::Identity();
			tranform.translate(Eigen::Vector3f(start_x + abs(glyph.bearX*scale), start_y*scale + glyph.bearY*scale, 0));
			tranform.scale(Eigen::Vector3f(scale_x, scale_y, 1));
			tranform.translate(Eigen::Vector3f(1, -1, 0));
			Eigen::Matrix4f m = tranform.matrix();
			auto p = m.data();
			m_instacePos->setTuple(idx, p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7], p[8], p[9], p[10], p[11], p[12], p[13], p[14], p[15]);

			start_x += glyph.Advance*scale;
			idx++;
		}
	}
}

void XTextItem::setTextScreenPos(int x, int y) {
	m_screenPos = myUtilty::Vec2i(x, y);
}

void XTextItem::setFontSize(int size) {
	m_fontSize = size;
}

int XTextItem::getFontSize() const {
	return m_fontSize;
}

myUtilty::Vec2i XTextItem::getTextScrrenPos()
{
	return m_screenPos;
}

void XTextItem::updateData()
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
	}


	if (m_textureArray && m_textureArray->GetTimeStamp() > m_UpdateTime) {

		m_textureBuffer->bind();
		m_textureBuffer->allocate(m_textureArray->data(0), m_textureArray->size());
		m_textureBuffer->release();
	}

	if (m_instacePos && m_instacePos->GetTimeStamp() > m_UpdateTime) {
		m_instanceAttrBufffer->bind();
		m_instanceAttrBufffer->allocate(m_instacePos->data(0), m_instacePos->size());
		m_instanceAttrBufffer->release();
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

uint32_t XTextItem::computeNumofVertices()
{
	if(m_drawType == PrimitveType::line_strip_adjacency)
		return m_coordArray->getNumOfTuple()+3;
	else
		return m_coordArray->getNumOfTuple();
}
