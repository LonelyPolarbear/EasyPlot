#include "xtextSource.h"
#include "dataBase/XTimeStamp.h"
#include "lib08_freetype/xfreetype.h"

class XTextSource::Internal {
public:
	
	sptr<XFloatArray> instanceTextCoordArray;
	XTimeStamp mConfigDataTimeStamp;

	std::wstring text;
	double fontSize = 64;
	bool isFixWidth = false;
	double fixedWidth = 10;

	HAlign alignment_h = HAlign::Left;
	VAlign alignment_v =VAlign::Top;

	void configDataModified() {
		mConfigDataTimeStamp.Modified();
	}
	bool isNeedUpdateConfig() {
		return mConfigDataTimeStamp >instanceTextCoordArray->GetTimeStamp();
	}
};

XTextSource::XTextSource():mData(new Internal)
{
	mData->instanceTextCoordArray = makeShareDbObject<XFloatArray>(); 
	mData->instanceTextCoordArray->setComponent(3);
	m_customArray.insert({0,mData->instanceTextCoordArray });
	Modified();
}

XTextSource::~XTextSource()
{

}

void XTextSource::updateVertextCoordArray()
{
	m_VertexCoord->setNumOfTuple(4);
	m_VertexCoord->setTuple(0, -1.0f, -1.0f, 0.0f);
	m_VertexCoord->setTuple(1, 1.0f, -1.0f, 0.0f);
	m_VertexCoord->setTuple(2, 1.0f, 1.0f, 0.0f);
	m_VertexCoord->setTuple(3, -1.0f, 1.0f, 0.0f);
	m_VertexCoord->Modified();
}

void XTextSource::updateFaceIndexArray()
{
	m_FaceIndexs->setNumOfTuple(2);
	m_FaceIndexs->setTuple(0, 0, 1, 2);
	m_FaceIndexs->setTuple(1, 0, 2, 3);
	m_FaceIndexs->Modified();
}

void XTextSource::updateCustomArray()
{
	if (mData->isNeedUpdateConfig()) {
		//更新纹理数据
		updateTexCoord();
	}
}

void XTextSource::setHasUpdated()
{
	if (xfreetype::Instance()->isSdfLoaded() == false) {
		return;
	}
	else {
		XShapeSource::setHasUpdated();
	}
}

void XTextSource::updateTexCoord()
{
	//
	if (xfreetype::Instance()->isSdfLoaded() == false) {
		//Modified();				//为了保证下次XTextSource的更新能够进来
		return;
	}
	auto num = mData->text.size();
	mData->instanceTextCoordArray->setNumOfTuple(num * 4);							//一个实例 四个vec3f 12个浮点数
	m_InstanceArray->setNumOfTuple(num);															//num个文字，num个实例

	//获取每个字符的纹理
	double start_x = 0;
	double scale = mData->fontSize / (double)64;		//字形的缩放系数

	double start_y = 0;

	auto fontSize = mData->fontSize;
	auto mIsFixWidth = mData->isFixWidth;
	auto mFixedWidth = mData->fixedWidth;
	auto text = mData->text;

	auto adjust = [this, scale, mIsFixWidth, mFixedWidth](double& start_x, double& start_y) {
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
	auto rowInfos = xfreetype::Instance()->computeLineNums(text, fontSize, mFixedWidth, mIsFixWidth);
	int rowNum = 0;
	for (int i = 0; i < num; i++) {
		auto c = text.at(i);
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

			mData->instanceTextCoordArray->setTuple(4 * i + 0, x_, y_, layer);
			mData->instanceTextCoordArray->setTuple(4 * i + 1, x_ + w_, y_, layer);
			mData->instanceTextCoordArray->setTuple(4 * i + 2, x_ + w_, y_ + h_, layer);
			mData->instanceTextCoordArray->setTuple(4 * i + 3, x_, y_ + h_, layer);

			/*
			mData->instanceTextCoordArray->setTuple(4 * i + 0,		0, 0, 0);
			mData->instanceTextCoordArray->setTuple(4 * i+	1 ,		1,0, 0);
			mData->instanceTextCoordArray->setTuple(4 * i + 2,		1, 1, 0);
			mData->instanceTextCoordArray->setTuple(4 * i + 3,		0, 1, 0);
			*/

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
				if (mData->alignment_h == HAlign::Center) {
					//offset_x = (mFixedWidth - rowLen) * 0.5;
					offset_x -= rowLen * 0.5;
				}
				else if (mData->alignment_h == HAlign::Right) {
					//offset_x = mFixedWidth - rowLen;
					offset_x -= rowLen;
				}
				//}
			}

			{
				if (mData->alignment_v == VAlign::Top) {
					auto aboveBaseLineLen = rowInfos->data(rowNum)[2];
					if (aboveBaseLineLen < 0) {
						int gggg = 10;
					}
					auto downBaseLineLen = rowInfos->data(rowNum)[3];
					//offset_y -= xfreetype::Instance()->getLineRowSpace() * scale;
					//offset_y -= (xfreetype::Instance()->getLineRowSpace() - aboveBaseLineLen) * scale;
					offset_y -= aboveBaseLineLen * scale;
				}

				else if (mData->alignment_v == VAlign::Middle)
				{
					auto aboveBaseLineLen = rowInfos->data(rowNum)[2];
					auto downBaseLineLen = rowInfos->data(rowNum)[3];
					offset_y -= (aboveBaseLineLen - downBaseLineLen) * 0.5 * scale;
				}
			}
			tranform.translate(Eigen::Vector3f(start_x + bearx * scale + offset_x, start_y + glyph.bearY * scale + offset_y, 0));
			tranform.scale(Eigen::Vector3f(scale_x, scale_y, 1));
			tranform.translate(Eigen::Vector3f(1, -1, 0));
			Eigen::Matrix4f m = tranform.matrix();
			auto p = m.data();
			m_InstanceArray->setTuple(i, p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7], p[8], p[9], p[10], p[11], p[12], p[13], p[14], p[15]);
			

			if (c != '\n') {
				start_x += glyph.Advance * scale;
			}
		}
	}

	mData->instanceTextCoordArray->Modified();
	m_InstanceArray->Modified();
}

void XTextSource::setText(const std::wstring& text)
{
	//根据文本，更新纹理坐标信息
	if (mData->text == text) {
		return;
	}
	mData->text = text;
	mData->configDataModified();
	Modified();
}

void XTextSource::setFontSize(double fontSize)
{
	mData->fontSize = fontSize;
	mData->configDataModified();
	Modified();
}

void XTextSource::setFixedWidth(double width)
{
	mData->fixedWidth = width;
	mData->configDataModified();
	Modified();
}

void XTextSource::setIsFixed(bool isFixed)
{
	mData->isFixWidth = isFixed;
	mData->configDataModified();
	Modified();
}

void XTextSource::setTextHorAlignment(HAlign alignment)
{
	mData->alignment_h = alignment;
	mData->configDataModified();
	Modified();
}

void XTextSource::setTextVerAlignment(VAlign alignment)
{
	mData->alignment_v = alignment;
	mData->configDataModified();
	Modified();
}

std::wstring XTextSource::getText() const
{
	return mData->text;
}

double XTextSource::getFontSize() const
{
	return mData->fontSize;
}

double XTextSource::getFixedWidth() const
{
	return mData->fixedWidth;
}

bool XTextSource::isFixedWidth() const
{
	return mData->isFixWidth;
}

HAlign XTextSource::getTextHorAlignment() const
{
	return mData->alignment_h;
}

VAlign XTextSource::getTextVerAlignment() const
{
	return mData->alignment_v;
}
