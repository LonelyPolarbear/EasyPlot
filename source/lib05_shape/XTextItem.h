#pragma once
#include "XGraphicsItem.h"

class LIB05_SHAPE_API XTextItem :public XGraphicsItem {
public:
	enum class HAlign {
		Left,
		Center,
		Right
	};
	enum class VAlign {
		Top,
		Middle,
		Bottom
	};
	XTextItem();
	virtual ~XTextItem();
	void draw(const Eigen::Matrix4f& m) override;
	 void initResource() override;
	 void setText(const std::wstring& text);
	 void setFontSize(double size);
	 double getFontSize() const;
	 double getFixedWidth() const;
	 void updateText();
	 virtual void updateData();

	 void setIsFixWidth(bool isFixWidth);
	 void setFixedWidth(double fixedWidth);

	 void setVAlignment(VAlign valignment);
	 void setHAlignment(HAlign halignment);
	 VAlign getVAlignment() const;
	 HAlign getHAlignment() const;
	 bool isFixWidth() const;

	 std::wstring getText() const {
		  return m_text;
	 }
protected:
	virtual uint32_t computeNumofVertices() override;
private:
	void configDataModified();
	void adjustHAligment(int instanceStartIdx,int instanceEndIdx,int curRowLen);
	std::shared_ptr<XFloatArray> m_textureArray;
	std::shared_ptr<XOpenGLBuffer> m_textureBuffer;


	double m_fontSize = 64;
	std::wstring m_text;
	bool mIsFixWidth = false;
	double mFixedWidth = 10;
	VAlign mValignment = VAlign::Bottom;
	HAlign mHAlignment = HAlign::Left;
	XTimeStamp mConfigDataTimeStamp;		//上述信息的时间戳
};