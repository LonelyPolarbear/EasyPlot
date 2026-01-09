#include "XTextItem.h"
#include <lib04_opengl/XOpenGLBuffer.h>
#include <lib04_opengl/XOpenGLEnable.h>
#include <lib04_opengl/XOpenGLVertexArrayObject.h>
#include <lib01_shader/xshaderManger.h>
#include <lib08_freetype/xfreetype.h>
#include <Eigen/Eigen>

XTextItem::XTextItem(std::shared_ptr<XGraphicsItem> parent) :XGraphicsItem(parent)
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
	index->setTuple(0, 0, 1, 2);
	index->setTuple(1, 0, 2, 3);

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
	initiallize();
	if (!m_IsVisible || !m_shaderManger)
		return;

	auto glEnableObj = makeShareDbObject<XOpenGLEnable>();

	//!
	//! [1] 填充绘制
	if (m_IsFilled) {
		glEnableObj->enable(XOpenGLEnable::EnableType::BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnableObj->disable(XOpenGLEnable::EnableType::DEPTH_TEST);

		//提取平移和旋转，去除缩放
		Eigen::Affine3f transform = Eigen::Affine3f::Identity();
		transform.matrix() = m;

		auto data = XQ::Matrix::transformDecomposition_TRS(transform);
		auto tmp = data;
		tmp.sx = 1;
		tmp.sy = 1;
		tmp.sz = 1;

		auto mat = XQ::Matrix::computeMatrix(tmp);

		drawFill(m_shaderManger->getTextShader(), m);
		glEnableObj->restore();
	}
}

void XTextItem::pickBorderDraw(std::shared_ptr<xshader> shader, const Eigen::Matrix4f& m)
{
	return XGraphicsItem::pickBorderDraw(shader, m);
}

void XTextItem::pickFillDraw(std::shared_ptr<xshader> shader, const Eigen::Matrix4f& m)
{
	return XGraphicsItem::pickFillDraw(shader, m);
}

void XTextItem::initResource()
{
	XGraphicsItem::initResource();

	//设置纹理属性
	m_vao->bind();

	m_textureBuffer->setBufferType(XOpenGLBuffer::VertexBuffer);

	m_textureBuffer->setUsagePattern(XOpenGLBuffer::StaticDraw);

	m_textureBuffer->create();

	m_vao->addBuffer(5, m_textureBuffer, 3, XOpenGL::DataType::float_, sizeof(float) * 12, sizeof(float) * 3 * 0, true);
	m_vao->addBuffer(6, m_textureBuffer, 3, XOpenGL::DataType::float_, sizeof(float) * 12, sizeof(float) * 3 * 1, true);
	m_vao->addBuffer(7, m_textureBuffer, 3, XOpenGL::DataType::float_, sizeof(float) * 12, sizeof(float) * 3 * 2, true);
	m_vao->addBuffer(8, m_textureBuffer, 3, XOpenGL::DataType::float_, sizeof(float) * 12, sizeof(float) * 3 * 3, true);

	m_vao->release();
}

void XTextItem::setText(const std::wstring& text)
{
	if (text == m_text)
	{
		return;
	}
	m_text = text;
	//updateText();
	configDataModified();
}

void XTextItem::setFontSize(double size) {
	m_fontSize = size;
	if (m_text.size())
		//updateText();
		configDataModified();
}

double XTextItem::getFontSize() const {
	return m_fontSize;
}

double XTextItem::getFixedWidth() const {
	return mFixedWidth;
}

void XTextItem::updateText()
{

if(mConfigDataTimeStamp < m_UpdateTime)
	return;

	mConfigDataTimeStamp.Modified();

	//设置实例化属性
	m_instacePos = makeShareDbObject<XFloatArray>();
	m_instacePos->setComponent(16);

	auto num = m_text.size();
	m_instacePos->setNumOfTuple(num);
	m_textureArray->setNumOfTuple(num * 4);
	m_textureArray->Modified();
	//获取每个字符的纹理
	double start_x = 0;
	double scale = (double)m_fontSize / (double)64;		//字形的缩放系数

	double start_y = 0;

	auto adjust = [this, scale](double& start_x, double& start_y) {
		if (mIsFixWidth == false) {
			return false;
		}
		if (start_x > mFixedWidth) {
			//start_x = 0;
			start_y -= xfreetype::Instance()->getLineRowSpace() * scale;
			return true;
		}
		return false;
		};

	//首先计算每一行的字符数
	auto rowInfos = xfreetype::Instance()->computeLineNums(m_text, m_fontSize,mFixedWidth,mIsFixWidth);
	int rowNum = 0;
	for (int i = 0; i < num; i++) {
		auto c = m_text.at(i);
		{
			auto glyph = xfreetype::Instance()->getCharacterSdf(c);

			double tmp = start_x + glyph.Advance * scale;
			if (adjust(tmp, start_y)) {
				start_x = 0;
				rowNum++;
			}

			auto layer = glyph.layer;
			auto width = glyph.width;
			auto height = glyph.height;

			auto picture_width = xfreetype::Instance()->getSdfPictureWidth();
			auto picture_height = xfreetype::Instance()->getSdfPictureWidth();
			auto fontBlockWidth = xfreetype::Instance()->getSdfSingleTextWidth();
			auto fontBlockHeight = xfreetype::Instance()->getSdfSingleTextHeight();

			auto x = glyph.x * fontBlockWidth;		//字形位置
			auto y = glyph.y * fontBlockHeight;
			y += (fontBlockHeight - height);
			//y +=  height;

			auto x_ = (float)x / picture_width;
			auto y_ = (float)y / picture_height;
			auto w_ = (float)width / picture_width;
			auto h_ = (float)height / picture_height;

			m_textureArray->setTuple(4 * i + 0, x_, y_, layer);
			m_textureArray->setTuple(4 * i + 1, x_ + w_, y_, layer);
			m_textureArray->setTuple(4 * i + 2, x_ + w_, y_ + h_, layer);
			m_textureArray->setTuple(4 * i + 3, x_, y_ + h_, layer);

			//字符位置

			float scale_x = glyph.width * 0.5 * scale;
			float scale_y = glyph.height * 0.5 * scale;

			Eigen::Affine3f tranform = Eigen::Affine3f::Identity();
			//int bearx = start_x == 0 ? abs(glyph.bearX) : glyph.bearX;
			int bearx = glyph.bearX;

			if (c == '\n') {
				//前一行绘制结束，根据水平对齐方式重新调整上一行的起始位置
				start_x = 0;
				start_y -= xfreetype::Instance()->getLineRowSpace() * scale;
				rowNum++;
			}

			//获取该行的信息
			double offset_x = 0;
			double offset_y = 0;
			{	
				//if (mIsFixWidth) {
					auto rowLen = rowInfos->data(rowNum)[1];
					if (getHAlignment() == HAlign::Center) {
						//offset_x = (mFixedWidth - rowLen) * 0.5;
						offset_x -= rowLen * 0.5;
					}
					else if (getHAlignment() == HAlign::Right) {
						//offset_x = mFixedWidth - rowLen;
						offset_x -=   rowLen;
					}
				//}
			}

			{
				if (mValignment == VAlign::Top) {
					auto aboveBaseLineLen = rowInfos->data(rowNum)[2];
					if (aboveBaseLineLen < 0) {
						int gggg =10;
					}
					auto downBaseLineLen = rowInfos->data(rowNum)[3];
					//offset_y -= xfreetype::Instance()->getLineRowSpace() * scale;
					//offset_y -= (xfreetype::Instance()->getLineRowSpace() - aboveBaseLineLen) * scale;
					offset_y -= aboveBaseLineLen * scale;
				}
					
				else if (mValignment == VAlign::Middle)
				{
					auto aboveBaseLineLen = rowInfos->data(rowNum)[2];
					auto downBaseLineLen = rowInfos->data(rowNum)[3];
					offset_y -= (aboveBaseLineLen - downBaseLineLen)*0.5*scale;
				}
			}
			tranform.translate(Eigen::Vector3f(start_x + bearx * scale+ offset_x, start_y + glyph.bearY * scale+offset_y, 0));
			tranform.scale(Eigen::Vector3f(scale_x, scale_y, 1));
			tranform.translate(Eigen::Vector3f(1, -1, 0));
			Eigen::Matrix4f m = tranform.matrix();
			auto p = m.data();
			m_instacePos->setTuple(i, p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7], p[8], p[9], p[10], p[11], p[12], p[13], p[14], p[15]);

			if (c != '\n') {
				start_x += glyph.Advance * scale;
			}
		}
	}
}

void XTextItem::updateData()
{
	updateText();
	//顶点数据已经更新
	updateVboCoord();

	updateVboColor();

	updateVboEbo();


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

	m_UpdateTime.Modified();
}

void XTextItem::setIsFixWidth(bool isFixWidth)
{
	mIsFixWidth = isFixWidth;
	if (m_text.size())
		//updateText();
		configDataModified();
}

void XTextItem::setFixedWidth(double fixedWidth)
{
	mFixedWidth = fixedWidth;
	if (m_text.size())
		//updateText();
		configDataModified();
}

void XTextItem::setVAlignment(VAlign valignment)
{
	mValignment = valignment;
	if (m_text.size())
		//updateText();
		configDataModified();
}

void XTextItem::setHAlignment(HAlign halignment)
{
	mHAlignment = halignment;
	if (m_text.size())
		//updateText();
		configDataModified();
}

XTextItem::VAlign XTextItem::getVAlignment() const
{
	return mValignment;
}

XTextItem::HAlign XTextItem::getHAlignment() const
{
	return mHAlignment;
}

bool XTextItem::isFixWidth() const
{
	return mIsFixWidth;
}

uint32_t XTextItem::computeNumofVertices()
{
	if (m_drawType == PrimitveType::line_strip_adjacency)
		return m_coordArray->getNumOfTuple() + 3;
	else
		return m_coordArray->getNumOfTuple();
}

void XTextItem::configDataModified()
{
	mConfigDataTimeStamp.Modified();
}

void XTextItem::adjustHAligment(int instanceStartIdx, int instanceEndIdx, int curRowLen)
{
	if (getHAlignment() == HAlign::Center) {
		//当前行的总长度=start_x
		auto offset_ = (mFixedWidth - curRowLen) * 0.5;
		for (int instaceIdx = instanceStartIdx; instaceIdx < instanceEndIdx; instaceIdx++) {
			auto pMatrix = m_instacePos->data(instaceIdx);
			Eigen::Matrix4f mat;
			memcpy(mat.data(), pMatrix, 16 * sizeof(float));
			Eigen::Matrix4f translate = Eigen::Matrix4f::Identity();
			translate.col(3) = Eigen::Vector4f(offset_, 0, 0, 1);
			mat = translate * mat;
			memcpy(pMatrix, mat.data(), 16 * sizeof(float));
		}
	}
	else if (getHAlignment() == HAlign::Right) {
		auto offset_ = mFixedWidth - curRowLen;
		for (int instaceIdx = instanceStartIdx; instaceIdx < instanceEndIdx; instaceIdx++) {
			auto pMatrix = m_instacePos->data(instaceIdx);
			Eigen::Matrix4f mat;
			memcpy(mat.data(), pMatrix, 16 * sizeof(float));
			Eigen::Matrix4f translate = Eigen::Matrix4f::Identity();
			translate.col(3) = Eigen::Vector4f(offset_, 0, 0, 1);
			mat = translate * mat;
			memcpy(pMatrix, mat.data(), 16 * sizeof(float));
		}
	}
	else if (getHAlignment() == HAlign::Left) {
		auto offset_ = 0;
		for (int instaceIdx = instanceStartIdx; instaceIdx < instanceEndIdx; instaceIdx++) {
			auto pMatrix = m_instacePos->data(instaceIdx);
			Eigen::Matrix4f mat;
			memcpy(mat.data(), pMatrix, 16 * sizeof(float));
			Eigen::Matrix4f translate = Eigen::Matrix4f::Identity();
			translate.col(3) = Eigen::Vector4f(offset_, 0, 0, 1);
			mat = translate * mat;
			memcpy(pMatrix, mat.data(), 16 * sizeof(float));
		}
	}
}
