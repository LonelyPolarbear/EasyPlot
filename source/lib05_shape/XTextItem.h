#pragma once
#include "XGraphicsItem.h"


class LIB05_SHAPE_API XTextItem :public XGraphicsItem {
public:
	
	XTextItem();
	virtual ~XTextItem();
	void draw(const Eigen::Matrix4f& m) override;
	 void initResource() override;
	 void setText(const std::wstring& text);
protected:
	virtual void updateData();
	virtual uint32_t computeNumofVertices() override;
private:
	std::shared_ptr<XFloatArray> m_textureArray;		//ÊµÀý»¯ÊôÐÔ
	std::shared_ptr<XOpenGLBuffer> m_textureBuffer;
};