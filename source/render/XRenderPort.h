#pragma once
#include "renderApi.h"
#include <dataBase/XDataBaseObject.h>
#include <dataBase/XDataObject.h>
#include <dataBase/XClolor.h>
#include <lib00_utilty/XUtilty.h>

class RENDER_API XRenderPort :public XDataObject {
	REGISTER_CLASS_META_DATA(XRenderPort, XDataObject);
protected:
	XRenderPort();
	~XRenderPort();

public:
	void setBackGroundColor1(const XQ::XColor& color);
	void setBackGroundColor2(const XQ::XColor& color);
	void setBackGroundColor1(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
	void setBackGroundColor2(unsigned char r, unsigned char g, unsigned char b, unsigned char a);

	XQ::XColor getBackGroundColor1() const;
	XQ::XColor getBackGroundColor2() const;

	/// <summary>
	/// 设置视口大小 0-1，x y w h,原点位于窗口左下角
	/// </summary>
	void setViewPort(const XQ::Rectd& viewPort);
	void setViewPort(double x,double y,double w,double h);
	XQ::Rectd getViewPort() const;
protected:
	XQ::XColor m_backgroundColor1;
	XQ::XColor m_backgroundColor2;
	XQ::Rectd m_viewPort = XQ::Rectd(0,0,1,1);
};