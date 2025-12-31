#include "XOpenGLRenderWindow.h"
#include "lib04_opengl/XOpenGLContext.h"
#include "XRenderWindowInteractor.h"
#include "XRender.h"
#include <glew/glew.h>

XOpenGLRenderWindow::XOpenGLRenderWindow()
{
}

XOpenGLRenderWindow::~XOpenGLRenderWindow()
{
}

void XOpenGLRenderWindow::SetWindowId(uint64_t winId)
{
	if (!m_context->isValid()) {
		m_context->create(winId);
	}
}

void XOpenGLRenderWindow::Init()
{
    m_context =makeShareDbObject<XOpenGLContext>();
	m_interactor = makeShareDbObject<XRenderWindowInteractor>();
	m_interactor->setRenderWindow(asDerived<XOpenGLRenderWindow>());

	bindSigalSlot();
	xsig::connect(m_interactor,&XRenderWindowInteractor::SigResize,this,&XOpenGLRenderWindow::slotUpdateSize);
}

void XOpenGLRenderWindow::bindSigalSlot()
{
}

void XOpenGLRenderWindow::render()
{
	for (auto r : m_renders) {
		r->render();
	}

	makeCurrent();
	swapBuffers();
	doneCurrent();
}

bool XOpenGLRenderWindow::makeCurrent()
{
	return m_context->makeCurrent();
}

void XOpenGLRenderWindow::doneCurrent()
{
	return m_context->doneCurrent();
}

void XOpenGLRenderWindow::swapBuffers()
{
	return m_context->swapBuffers();
}

sptr<XOpenGLContext> XOpenGLRenderWindow::getContext() const
{
	return m_context;
}

void XOpenGLRenderWindow::slotUpdateSize(XQ::Vec2i size)
{
	m_windowSize.setValues(size[0],size[1]);
	//更新渲染视口
	makeCurrent();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, size[0], size[1]);
	doneCurrent();
}

int XOpenGLRenderWindow::getWindowWidth() const
{
	return m_windowSize.x();
}

int XOpenGLRenderWindow::getWindowHeight() const
{
	return m_windowSize.y();
}

sptr<XRenderWindowInteractor> XOpenGLRenderWindow::getInteractor() const
{
	return m_interactor;
}

void XOpenGLRenderWindow::addRender(sptr<XRender> ren)
{
	if(!ren)
		return;
	m_renders.push_back(ren);
}
