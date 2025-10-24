#include <glew/glew.h>
#include<windows.h>

#include "XOpenGLWidget.h"
#include <QOpenGLContext>
#include <QWindow>
#include <QResizeEvent>

#include <QDebug>
#include <iostream>
#include <chrono>

#include <glew/wglew.h>
#include "lib04_opengl/XOpenGLContext.h"

using namespace std::chrono_literals;

XOpenGLWidget::XOpenGLWidget(QWidget* parent) :QWidget(parent)
{
	// 设置 Widget 属性
	setAttribute(Qt::WA_PaintOnScreen, true); // 禁用 Qt 的绘制缓冲
	setAttribute(Qt::WA_NativeWindow, true);	// 强制使用原生窗口句柄
	setAttribute(Qt::WA_NoSystemBackground, true);
	setAttribute(Qt::WA_OpaquePaintEvent, true);

	this->startTimer(60ms);

	mContext = makeShareDbObject<XOpenGLContext>();
}

XOpenGLWidget::~XOpenGLWidget()
{
	makeCurrent();
	doneCurrent();
}

bool XOpenGLWidget::initOpenglContext()
{
	return mContext->create(winId());
}

WId XOpenGLWidget::nativeHandle()
{
	return winId();
}

bool XOpenGLWidget::makeCurrent()
{
	return mContext->makeCurrent();
}

void XOpenGLWidget::doneCurrent()
{
	mContext->doneCurrent();
}

void XOpenGLWidget::swapBuffers()
{
	return mContext->swapBuffers();
}

void XOpenGLWidget::resizeEvent(QResizeEvent* event)
{
	// 窗口大小改变事件处理
	mWidth = event->size().width();
	mHeight = event->size().height();
	if (m_isInit) {
		makeCurrent();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, event->size().width(), event->size().height());
		doneCurrent();
	}
	return QWidget::resizeEvent(event);
}

void XOpenGLWidget::timerEvent(QTimerEvent* event)
{
	// 定时器事件处理
	if (!m_isInit)
		return;

	render();
}

void XOpenGLWidget::showEvent(QShowEvent* event)
{
	if (m_isInit == false) {
		//if (testAttribute(Qt::WA_WState_Created)) {
		initOpenglContext();
		m_isInit = true;
		initGLResource();
		//}
	}
	return QWidget::showEvent(event);
}

//QWidget::paintEngine: Should no longer be called
QPaintEngine* XOpenGLWidget::paintEngine() const
{
	return nullptr;
}

void XOpenGLWidget::render()
{
	makeCurrent();
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0.2f, 0.0f, 0.8f, 1.0f);

	swapBuffers();
	glUseProgram(0);
	doneCurrent();
}

std::shared_ptr<XOpenGLContext> XOpenGLWidget::getContext() const{
	return mContext;
}

myUtilty::Vec2u  XOpenGLWidget::mapToGLScreen(const QPoint& point) const{
	myUtilty::Vec2u p;
	p.x =point.x();
	p.y =mHeight- point.y();
	return p;
}

myUtilty::Vec2f  XOpenGLWidget::mapToNormGLScreen(const QPoint& point) const{
	myUtilty::Vec2f p;
	p.x =(float)point.x() /(float) mWidth;
	p.y =(float)(mHeight - point.y())/(float) mHeight;
	return p;
}