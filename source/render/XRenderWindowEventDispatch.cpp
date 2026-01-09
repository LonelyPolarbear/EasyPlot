#include "XRenderWindowEventDispatch.h"

XRenderWindowEventDispatch::XRenderWindowEventDispatch()
{
}

XRenderWindowEventDispatch::~XRenderWindowEventDispatch()
{
}

void XRenderWindowEventDispatch::setRenderWindow(sptr<XOpenGLRenderWindow> ren)
{
	m_renderWindow = ren;
}

sptr<XOpenGLRenderWindow> XRenderWindowEventDispatch::getRenderWindow() const
{
	return m_renderWindow.lock();
}
