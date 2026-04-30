#include "XRenderPort.h"

XRenderPort::XRenderPort()
{
}

XRenderPort::~XRenderPort()
{
}

void XRenderPort::Init()
{
	XDataObject::Init();
	XQ::XColor bot_color = XQ::XColor::from_normalcolor(0.914, 0.925, 0.937, 1);
	XQ::XColor top_color = XQ::XColor::from_normalcolor(0.973, 0.976, 0.980, 1);
	XQ_ATTR_ADD_INIT(AttrBottomColor, bot_color);
	XQ_ATTR_ADD_INIT(AttrTopColor, top_color);
	XQ_ATTR_ADD_INIT(AttrViewPort, XQ::Rectf(0, 0, 1, 1));
}

void XRenderPort::setBackGroundColorTop(const XQ::XColor& color)
{
	AttrTopColor->setValue(color);
}

void XRenderPort::setBackGroundColorTop(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	AttrTopColor->setValue(XQ::XColor(r,g,b,a));
}

void XRenderPort::setBackGroundColorBot(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	AttrBottomColor->setValue(XQ::XColor(r, g, b, a));
}

void XRenderPort::setBackGroundColorBot(const XQ::XColor& color)
{
	AttrBottomColor->setValue(color);
}

XQ::XColor XRenderPort::getBackGroundColorTop() const
{
	return AttrTopColor->getValue();
}

XQ::XColor XRenderPort::getBackGroundColorBot() const
{
	return AttrBottomColor->getValue();
}

void XRenderPort::setViewPort(const XQ::Rectf& viewPort)
{
	AttrViewPort->setValue(viewPort);
}

void XRenderPort::setViewPort(float x, float y, float w, float h)
{
	AttrViewPort->setValue(XQ::Rectf(x,y,w,h));
}

XQ::Rectf XRenderPort::getViewPort() const
{
	return AttrViewPort->getValue();
}