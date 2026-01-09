#include <glew/glew.h>
#include<windows.h>

#include "XGLWidget.h"
#include <QOpenGLContext>
#include <QWindow>
#include <QResizeEvent>

#include <iostream>
#include <chrono>

#include <glew/wglew.h>
#include "lib04_opengl/XOpenGLContext.h"
#include "lib02_camera/xcamera.h"
#include "lib05_shape/xchamferCubeSource.h"
#include "lib05_shape/xconeSource.h"
#include "lib05_shape/xshape.h"
#include "render/XOpenGLRenderWindow.h"
#include "render/XRenderWindowEventDispatch.h"
#include "render/XRender.h"
#include "render/XRenderCamera.h"

using namespace std::chrono_literals;

XGLWidget::XGLWidget(QWidget* parent) :QWidget(parent)
{
	// 设置 Widget 属性
	setAttribute(Qt::WA_PaintOnScreen, true); // 禁用 Qt 的绘制缓冲
	setAttribute(Qt::WA_NativeWindow, true);	// 强制使用原生窗口句柄
	setAttribute(Qt::WA_NoSystemBackground, true);
	setAttribute(Qt::WA_OpaquePaintEvent, true);
	this->setMouseTracking(true);
	this->startTimer(60ms);

	mRenderWindow = makeShareDbObject<XOpenGLRenderWindow>();
}

XGLWidget::~XGLWidget()
{
}

std::shared_ptr<XOpenGLRenderWindow> XGLWidget::getRenderWindow()
{
	return mRenderWindow;
}

void XGLWidget::renderTest()
{
#if 1
	{
		auto render = makeShareDbObject<XRender>();
		render->setActive(false);
		render->setBackGroundColor1(255, 0, 0, 255);
		render->setViewPort(0, 0, 0.5, 1);
		mRenderWindow->addRender(render);

		sptr<xchamferCubeSource> cubeSource = makeShareDbObject<xchamferCubeSource>();

		sptr<XShape> cubeActor = makeShareDbObject<XShape>();

		cubeActor->setColorMode(ColorMode::FaceColor);

		cubeActor->setPolygonMode(PolygonMode::fill);

		cubeActor->setSingleColor(XQ::Vec4f(0, 0, 1, 1));

		cubeActor->setInput(cubeSource);

		cubeSource->Modified();

		render->addActor3D(cubeActor);

		render->fitView();
	}
#endif
#if 1
	{
		auto render2 = makeShareDbObject<XRender>();
		render2->setActive(false);
		render2->setBackGroundColor1(0, 255, 0, 255);
		render2->setViewPort(0.5, 0, 0.5, 1);
		render2->getCamera()->getInnerCamera()->setType(xcamera::cameraType::perspective);
		mRenderWindow->addRender(render2);

		sptr<xchamferCubeSource> cubeSource = makeShareDbObject<xchamferCubeSource>();

		sptr<XShape> cubeActor = makeShareDbObject<XShape>();

		cubeActor->setColorMode(ColorMode::SingleColor);

		cubeActor->setSingleColor(XQ::Vec4f(0, 0, 1, 1));

		cubeActor->setInput(cubeSource);

		cubeSource->Modified();

		render2->addActor3D(cubeActor);

		render2->fitView();
	}
#endif
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

bool XGLWidget::initOpenglContext()
{
	mRenderWindow->SetWindowId(winId());
	return true;
}

void XGLWidget::enterEvent(QEvent* event)
{
	if (mRenderWindow) {
		auto eventDispatcher = mRenderWindow->getEventDispatcher();
		eventDispatcher->SigEnter();
	}
	return QWidget::enterEvent(event);
}

void XGLWidget::leaveEvent(QEvent* event)
{
	if (mRenderWindow) {
		auto eventDispatcher = mRenderWindow->getEventDispatcher();
		eventDispatcher->SigLeave();
	}
	return QWidget::leaveEvent(event);
}

void XGLWidget::mousePressEvent(QMouseEvent* event)
{
	if (mRenderWindow) {
		const QEvent::Type t = event->type();
		auto modifiers =(uint32_t) event->modifiers();
		XQ::KeyboardModifier m =(XQ::KeyboardModifier) modifiers;
		auto eventDispatcher = mRenderWindow->getEventDispatcher();
		switch (event->button())
		{
		case Qt::LeftButton:
			eventDispatcher->SigLeftButtonPress(mapToGLScreen(event->pos()),m);
			break;

		case Qt::MidButton:
			eventDispatcher->SigMiddleButtonPress(mapToGLScreen(event->pos()),m );
			break;

		case Qt::RightButton:
			eventDispatcher->SigRightButtonPress(mapToGLScreen(event->pos()), m);
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
		mRenderWindow->getEventDispatcher()->SigMouseMove(mapToGLScreen(event->pos()), m);
	}
}

void XGLWidget::mouseReleaseEvent(QMouseEvent* event)
{
	if (mRenderWindow) {
		const QEvent::Type t = event->type();
		auto modifiers = (uint32_t)event->modifiers();
		XQ::KeyboardModifier m = (XQ::KeyboardModifier)modifiers;
		auto eventDispatcher = mRenderWindow->getEventDispatcher();
		switch (event->button())
		{
		case Qt::LeftButton:
			eventDispatcher->SigLeftButtonRelease(mapToGLScreen(event->pos()), m);
			break;

		case Qt::MidButton:
			eventDispatcher->SigMiddleButtonRelease(mapToGLScreen(event->pos()), m);
			break;

		case Qt::RightButton:
			eventDispatcher->SigRightButtonRelease(mapToGLScreen(event->pos()), m);
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
		auto eventDispatcher = mRenderWindow->getEventDispatcher();
		eventDispatcher->SigKeyPress(key, modifiers);
	}
	return QWidget::keyPressEvent(event);
}

void XGLWidget::keyReleaseEvent(QKeyEvent* event)
{
	XQ::Key key = (XQ::Key)((uint32_t)event->key());
	XQ::KeyboardModifier modifiers = (XQ::KeyboardModifier)((uint32_t)event->modifiers());
	if (mRenderWindow) {
		auto eventDispatcher = mRenderWindow->getEventDispatcher();
		eventDispatcher->SigKeyRelease(key, modifiers);
	}
	return QWidget::keyReleaseEvent(event);
}

void XGLWidget::focusInEvent(QFocusEvent* event)
{
	if (mRenderWindow) {
		auto eventDispatcher = mRenderWindow->getEventDispatcher();
		eventDispatcher->SigFoucsIn();
	}
	return QWidget::focusInEvent(event);
}

void XGLWidget::focusOutEvent(QFocusEvent* event)
{
	if (mRenderWindow) {
		auto eventDispatcher = mRenderWindow->getEventDispatcher();
		eventDispatcher->SigFoucsOut();
	}
	return QWidget::focusOutEvent(event);
}

void XGLWidget::wheelEvent(QWheelEvent* event)
{
	auto angle = event->angleDelta().y();
	if (mRenderWindow) {
		XQ::KeyboardModifier modifiers = (XQ::KeyboardModifier)((uint32_t)event->modifiers());
		auto eventDispatcher = mRenderWindow->getEventDispatcher();
		if (angle > 0) {
			eventDispatcher->SigMouseWheelForward(mapToGLScreen(event->pos()),modifiers);
		}
		else {
			eventDispatcher->SigMouseWheelBackward(mapToGLScreen(event->pos()), modifiers);
		}
	}
	return QWidget::wheelEvent(event);
}

void XGLWidget::resizeEvent(QResizeEvent* event)
{
	// 窗口大小改变事件处理
	mWidth = event->size().width();
	mHeight = event->size().height();
	if (m_isInit) {
		if (mRenderWindow) {
			auto interactor = mRenderWindow->getEventDispatcher();
			interactor->SigResize(XQ::Vec2i(event->size().width(), event->size().height()));
		}
	}
	return QWidget::resizeEvent(event);
}

void XGLWidget::timerEvent(QTimerEvent* event)
{
	if (!m_isInit)
		return;

	render();
}

void XGLWidget::showEvent(QShowEvent* event)
{
	if (m_isInit == false) {
		initOpenglContext();
		m_isInit = true;
	}
	if (mRenderWindow) {
		auto interactor = mRenderWindow->getEventDispatcher();
		interactor->SigResize(XQ::Vec2i(width(), height()));
	}
	return QWidget::showEvent(event);
}