#include "XRenderPort.h"

XRenderPort::XRenderPort()
{
}

XRenderPort::~XRenderPort()
{
}

void XRenderPort::setBackGroundColor1(const XQ::XColor& color)
{
	m_backgroundColor1 = color;
}

void XRenderPort::setBackGroundColor1(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	m_backgroundColor1 = XQ::XColor(r,g,b,a);
}

void XRenderPort::setBackGroundColor2(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	m_backgroundColor2 = XQ::XColor(r, g, b, a);
}

void XRenderPort::setBackGroundColor2(const XQ::XColor& color)
{
	m_backgroundColor2 = color;
}

XQ::XColor XRenderPort::getBackGroundColor1() const
{
	return m_backgroundColor1;
}

XQ::XColor XRenderPort::getBackGroundColor2() const
{
	return m_backgroundColor2;
}

void XRenderPort::setViewPort(const XQ::Rectd& viewPort)
{
	m_viewPort = viewPort;
}

void XRenderPort::setViewPort(double x, double y, double w, double h)
{
	m_viewPort = XQ::Rectd(x,y,w,h);
}

XQ::Rectd XRenderPort::getViewPort() const
{
	return m_viewPort;
}