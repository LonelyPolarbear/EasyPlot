#include "XRenderWindowInteractor.h"

XRenderWindowInteractor::XRenderWindowInteractor()
{
}

XRenderWindowInteractor::~XRenderWindowInteractor()
{
}

void XRenderWindowInteractor::setRenderWindow(sptr<XOpenGLRenderWindow> ren)
{
	m_renderWindow = ren;
}

sptr<XOpenGLRenderWindow> XRenderWindowInteractor::getRenderWindow() const
{
	return m_renderWindow.lock();
}
