#include "XRender.h"
#include "XOpenGLRenderWindow.h"
#include "XRenderCamera.h"
#include <glew/glew.h>

XRender::XRender()
{
}

XRender::~XRender()
{
}

void XRender::setRenderWindow(sptr<XOpenGLRenderWindow> renderWindow)
{
	m_renderWindow = renderWindow;
}

sptr<XOpenGLRenderWindow> XRender::getRenderWindow()
{
	return m_renderWindow.lock();
}

void XRender::setCamera(sptr<XRenderCamera> camera)
{
	m_camera = camera;
}

sptr<XRenderCamera> XRender::getCamera() const
{
	return m_camera;
}

void XRender::render()
{
	makeCurrent();
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0.2f, 0.0f, 0.8f, 1.0f);

	glUseProgram(0);
	doneCurrent();
}

bool XRender::makeCurrent()
{
	auto renderWindow = getRenderWindow();
	if (renderWindow) {
		return renderWindow->makeCurrent();
	}
	return false;
}

void XRender::doneCurrent()
{
	auto renderWindow = getRenderWindow();
	if (renderWindow) {
		return renderWindow->doneCurrent();
	}
}