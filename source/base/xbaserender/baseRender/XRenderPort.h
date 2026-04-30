#pragma once
#include "base/xbaserender/xbaserenderApi.h"
#include <dataBase/XDataBaseObject.h>
#include <dataBase/XDataObject.h>
#include <dataBase/XClolor.h>
#include <lib00_utilty/XUtilty.h>

class XBASERENDER_API XRenderPort :public XDataObject {
	REGISTER_CLASS_META_DATA(XRenderPort, XDataObject);
protected:
	XRenderPort();
	~XRenderPort();
	void Init() override;
public:
	void setBackGroundColorTop(const XQ::XColor& color);
	void setBackGroundColorBot(const XQ::XColor& color);
	void setBackGroundColorTop(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
	void setBackGroundColorBot(unsigned char r, unsigned char g, unsigned char b, unsigned char a);

	XQ::XColor getBackGroundColorTop() const;
	XQ::XColor getBackGroundColorBot() const;

	/// <summary>
	/// 设置视口大小 0-1，x y w h,原点位于窗口左下角
	/// </summary>
	void setViewPort(const XQ::Rectf& viewPort);
	void setViewPort(float x, float y, float w, float h);
	XQ::Rectf getViewPort() const;
protected:
	csptr<XAttr_Color> AttrBottomColor;
	csptr<XAttr_Color> AttrTopColor;
	csptr<XAttr_Rectf> AttrViewPort;

	//XQ::Rectd m_viewPort = XQ::Rectd(0,0,1,1);
};