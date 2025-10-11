#pragma once
#include "XGraphicsItem.h"

class LIB05_SHAPE_API XTextItem :public XGraphicsItem {
public:
	
	XTextItem();
	virtual ~XTextItem();
	void draw(const Eigen::Matrix4f& m) override;
	 void initResource() override;
	 void setText(const std::wstring& text);
	// void setTextScreenPos(int x,int y);
	 void setFontSize(int size);
	 int getFontSize() const;
	 //myUtilty::Vec2i getTextScrrenPos();
	 void updateText();
	 virtual void updateData();
protected:
	virtual uint32_t computeNumofVertices() override;
private:
	std::shared_ptr<XFloatArray> m_textureArray;
	std::shared_ptr<XOpenGLBuffer> m_textureBuffer;
	//myUtilty::Vec2i m_screenPos;
	int m_fontSize = 64;
	std::wstring m_text;
};