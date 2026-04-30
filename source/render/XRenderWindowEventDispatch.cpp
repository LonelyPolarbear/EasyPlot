#include "XRenderWindowEventDispatch.h"
#include "base/xbaserender/baseRender/XBaseRenderWindow.h"
#include "XOpenGLRenderWindow.h"

XRenderWindowEventDispatch::XRenderWindowEventDispatch()
{
}

XRenderWindowEventDispatch::~XRenderWindowEventDispatch()
{
}

void XRenderWindowEventDispatch::setRenderWindow(sptr<XBaseRenderWindow> ren)
{
	m_renderWindow = ren->asDerived<XOpenGLRenderWindow>();
}

sptr<XBaseRenderWindow> XRenderWindowEventDispatch::getRenderWindow() const
{
	auto s= m_renderWindow.lock();
	return s;
}
