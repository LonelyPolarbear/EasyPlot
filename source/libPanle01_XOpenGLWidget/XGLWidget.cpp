#include <glew/glew.h>
#include<windows.h>

#include "XGLWidget.h"
#include <QOpenGLContext>
#include <QWindow>
#include <QResizeEvent>

#include <QDebug>
#include <iostream>
#include <chrono>

#include <glew/wglew.h>
#include "lib04_opengl/XOpenGLContext.h"
#include "render/XOpenGLRenderWindow.h"
#include "render/XRenderWindowInteractor.h"
#include "render/XRender.h"

using namespace std::chrono_literals;

XGLWidget::XGLWidget(QWidget* parent) :QWidget(parent)
{
	// 设置 Widget 属性
	setAttribute(Qt::WA_PaintOnScreen, true); // 禁用 Qt 的绘制缓冲
	setAttribute(Qt::WA_NativeWindow, true);	// 强制使用原生窗口句柄
	setAttribute(Qt::WA_NoSystemBackground, true);
	setAttribute(Qt::WA_OpaquePaintEvent, true);

	this->startTimer(60ms);

	mRenderWindow = makeShareDbObject<XOpenGLRenderWindow>();
	auto render =makeShareDbObject<XRender>();
	mRenderWindow->addRender(render);
	render->setRenderWindow(mRenderWindow);
}

XGLWidget::~XGLWidget()
{
	/*makeCurrent();
	doneCurrent();*/
}

bool XGLWidget::initOpenglContext()
{
	mRenderWindow->SetWindowId(winId());
	return true;
}

WId XGLWidget::nativeHandle()
{
	return winId();
}

bool XGLWidget::makeCurrent()
{
	return mRenderWindow->makeCurrent();
}

void XGLWidget::doneCurrent()
{
	return mRenderWindow->doneCurrent();
}

void XGLWidget::swapBuffers()
{
	return mRenderWindow->swapBuffers();
}

void XGLWidget::resizeEvent(QResizeEvent* event)
{
	// 窗口大小改变事件处理
	mWidth = event->size().width();
	mHeight = event->size().height();
	if (m_isInit) {
		/*makeCurrent();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, event->size().width(), event->size().height());
		doneCurrent();*/

		if (mRenderWindow) {
			auto interactor = mRenderWindow->getInteractor();
			interactor->SigResize(XQ::Vec2i(event->size().width(), event->size().height()));
		}
	}

	return QWidget::resizeEvent(event);
}

void XGLWidget::timerEvent(QTimerEvent* event)
{
	// 定时器事件处理
	if (!m_isInit)
		return;

	render();
}

void XGLWidget::showEvent(QShowEvent* event)
{
	if (m_isInit == false) {
		initOpenglContext();
		m_isInit = true;
		initGLResource();
	}
	return QWidget::showEvent(event);
}

//QWidget::paintEngine: Should no longer be called
QPaintEngine* XGLWidget::paintEngine() const
{
	return nullptr;
}

void XGLWidget::render()
{
	mRenderWindow->render();
}

std::shared_ptr<XOpenGLContext> XGLWidget::getContext() const{
	return mRenderWindow->getContext();
}

XQ::Vec2u  XGLWidget::mapToGLScreen(const QPoint& point) const{
	XQ::Vec2u p;
	p.x() = point.x();
	p.y() = mHeight - point.y();
	return p;
}

XQ::Vec2f  XGLWidget::mapToNormGLScreen(const QPoint& point) const{
	XQ::Vec2f p;
	p.x() = (float)point.x() / (float)mWidth;
	p.y() = (float)(mHeight - point.y()) / (float)mHeight;
	return p;
}

void XGLWidget::enterEvent(QEvent* event)
{
	if (mRenderWindow) {
		auto interactor = mRenderWindow->getInteractor();
		interactor->SigEnter();
	}
	return QWidget::enterEvent(event);
}

void XGLWidget::leaveEvent(QEvent* event)
{
	if (mRenderWindow) {
		auto interactor = mRenderWindow->getInteractor();
		interactor->SigLeave();
	}
	return QWidget::leaveEvent(event);
}

void XGLWidget::mousePressEvent(QMouseEvent* event)
{
	if (mRenderWindow) {
		const QEvent::Type t = event->type();
		auto modifiers =(uint32_t) event->modifiers();
		XQ::KeyboardModifier m =(XQ::KeyboardModifier) modifiers;
		auto interactor = mRenderWindow->getInteractor();
		switch (event->button())
		{
		case Qt::LeftButton:
			interactor->SigLeftButtonPress(mapToGLScreen(event->pos()),m);
			break;

		case Qt::MidButton:
			interactor->SigMiddleButtonPress(mapToGLScreen(event->pos()),m );
			break;

		case Qt::RightButton:
			interactor->SigRightButtonPress(mapToGLScreen(event->pos()), m);
			break;

		default:
			break;
		}
	}
	return QWidget::mousePressEvent(event);
}

void XGLWidget::mouseMoveEvent(QMouseEvent* event)
{
	if (mRenderWindow) {
		const QEvent::Type t = event->type();
		auto modifiers = (uint32_t)event->modifiers();
		XQ::KeyboardModifier m = (XQ::KeyboardModifier)modifiers;
		mRenderWindow->getInteractor()->SigMouseMove(mapToGLScreen(event->pos()), m);
	}
}

void XGLWidget::mouseReleaseEvent(QMouseEvent* event)
{
	if (mRenderWindow) {
		const QEvent::Type t = event->type();
		auto modifiers = (uint32_t)event->modifiers();
		XQ::KeyboardModifier m = (XQ::KeyboardModifier)modifiers;
		auto interactor = mRenderWindow->getInteractor();
		switch (event->button())
		{
		case Qt::LeftButton:
			interactor->SigLeftButtonRelease(mapToGLScreen(event->pos()), m);
			break;

		case Qt::MidButton:
			interactor->SigMiddleButtonRelease(mapToGLScreen(event->pos()), m);
			break;

		case Qt::RightButton:
			interactor->SigRightButtonRelease(mapToGLScreen(event->pos()), m);
			break;

		default:
			break;
		}
	}
	return QWidget::mouseReleaseEvent(event);
}

void XGLWidget::keyPressEvent(QKeyEvent* event)
{
	XQ::Key key= (XQ::Key)((uint32_t)event->key());
	XQ::KeyboardModifier modifiers = (XQ::KeyboardModifier)( (uint32_t)event->modifiers());
	if (mRenderWindow) {
		auto interactor = mRenderWindow->getInteractor();
		interactor->SigKeyPress(key, modifiers);
	}
	return QWidget::keyPressEvent(event);
}

void XGLWidget::keyReleaseEvent(QKeyEvent* event)
{
	XQ::Key key = (XQ::Key)((uint32_t)event->key());
	XQ::KeyboardModifier modifiers = (XQ::KeyboardModifier)((uint32_t)event->modifiers());
	if (mRenderWindow) {
		auto interactor = mRenderWindow->getInteractor();
		interactor->SigKeyRelease(key, modifiers);
	}
	return QWidget::keyReleaseEvent(event);
}

void XGLWidget::focusInEvent(QFocusEvent* event)
{
	if (mRenderWindow) {
		auto interactor = mRenderWindow->getInteractor();
		interactor->SigFoucsIn();
	}
	return QWidget::focusInEvent(event);
}

void XGLWidget::focusOutEvent(QFocusEvent* event)
{
	if (mRenderWindow) {
		auto interactor = mRenderWindow->getInteractor();
		interactor->SigFoucsOut();
	}
	return QWidget::focusOutEvent(event);
}

void XGLWidget::wheelEvent(QWheelEvent* event)
{
	auto angle = event->angleDelta().y();
	if (mRenderWindow) {
		XQ::KeyboardModifier modifiers = (XQ::KeyboardModifier)((uint32_t)event->modifiers());
		auto interactor = mRenderWindow->getInteractor();
		if (angle > 0) {
			interactor->SigMouseWheelForward(mapToGLScreen(event->pos()),modifiers);
		}
		else {
			interactor->SigMouseWheelBackward(mapToGLScreen(event->pos()), modifiers);
		}
	}

	return QWidget::wheelEvent(event);
}
