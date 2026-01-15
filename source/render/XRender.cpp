#include "XRender.h"
#include "XOpenGLRenderWindow.h"
#include "XRenderCamera.h"
#include "XRenderMultiModeInteractionHandler.h"
#include "CameraNavigationHandler.h"
#include "XRenderWindowEventDispatch.h"
#include "xsignal/XSignal.h"
#include "lib04_opengl/XOpenGLBuffer.h"
#include "lib04_opengl/XOpenGLEnable.h"
#include "lib04_opengl/XOpenGLFuntion.h"
#include <lib05_shape/XGeometryNode.h>
#include <lib05_shape/datasource/xCustomSource.h>
#include <lib05_shape/XGraphicsItem.h>
#include <lib02_camera/xcamera.h>


struct XRender::Internal {
	Internal(XRender *render) {
		m_camera = makeShareDbObject<XRenderCamera>();
		host = render;
	}

	wptr<XOpenGLRenderWindow> m_renderWindow;
	sptr<XInteractionEventHandler> m_multiModeEventHandler;
	sptr<XRenderCamera> m_camera;
	
	std::vector<sptr<XGeometryNode>> m_actor3DList;
	std::vector<sptr<XGraphicsItem>> m_actor2DList;

	XQ::Vec2f m_mousePos; //鼠标在窗口中的位置，未做变换
	bool m_isActive = false;
	XRender *host;


	sptr<RenderWindowUbo> getUbo() {
		return host->getRenderWindow()->getRenderWindowUbo();
	}

	void writeUbo() {
		getUbo()->create();
		//!
		//! 
		auto camera = m_camera;
		getUbo()->writeVS( camera->getViewMatrix(), camera->projectionMatrix());

		//!
		//! 
		auto view_port = host->getConvertViewPort();
		auto renderPos = m_mousePos;
		renderPos[0] -= view_port[0];
		renderPos[1] -= view_port[1];
		getUbo()->writeFS(XQ::Vec2f(view_port[2],view_port[3]), renderPos);		//视口宽高和鼠标位置
	}
};

XRender::XRender():mData(new Internal(this))
{
	
}

XRender::~XRender()
{
}

void XRender::Init()
{
	getOrCreateMultiModeEventHandler();
}

void XRender::setRenderWindow(sptr<XOpenGLRenderWindow> renderWindow)
{
	mData->m_renderWindow = renderWindow;
}

sptr<XOpenGLRenderWindow> XRender::getRenderWindow() const
{
	return mData->m_renderWindow.lock();
}

void XRender::setCamera(sptr<XRenderCamera> camera)
{
	mData->m_camera = camera;
}

sptr<XRenderCamera> XRender::getCamera() const
{
	return mData->m_camera;
}

void XRender::render()
{
	//renderwindow确保opengl上下文有效，因此此处可以不对makeCurrent返回值判断
	//所以所有actor需要初始化opengl的资源的接口都可以在此处完成
	makeCurrent();
	
	//!
	//! 
	updateViewPort();

	updateUbo();

	//!
	//! 渲染
	auto enable = makeShareDbObject<XOpenGLEnable>();
	enable->enable(XOpenGLEnable::EnableType::DEPTH_TEST);
	XOpenGLFuntion::xglClearDepth(1);
	XOpenGLFuntion::xglClear((unsigned int)XOpenGL::BufferBits::depth_buffer_bit);

	for (auto actor : mData->m_actor3DList) {
		actor->draw();
	}
	enable->restore();


	//glUseProgram(0);
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

bool XRender::setActive(bool isActive)
{
	bool tmp = mData->m_isActive;
	mData->m_isActive = isActive;
	return tmp;
}

bool XRender::isActive() const
{
	return mData->m_isActive;
}

void XRender::setCameraNavigationHandler(sptr<XInteractionEventHandler> interactionEventHandler)
{
	auto multiHandler =getOrCreateMultiModeEventHandler()->asDerived<XRenderMultiModeInteractionHandler>();
	if (multiHandler) {
		multiHandler->setCameraNavigationHandler(interactionEventHandler->asDerived<CameraNavigationHandler>());
	}
}

sptr<XInteractionEventHandler> XRender::getCameraNavigationHandler()
{
	auto multiHandler = getOrCreateMultiModeEventHandler()->asDerived<XRenderMultiModeInteractionHandler>();
	if (multiHandler) {
		return multiHandler->getCameraNavigationHandler();
	}
}

bool XRender::connectToRenderWindowSignals()
{
	auto handler = getOrCreateMultiModeEventHandler();

	//绑定信号槽
	auto eventDispatcher = getRenderWindow()->getEventDispatcher();
	if (!eventDispatcher) {
		return false;
	}

	xsig::connect(eventDispatcher, &XRenderWindowEventDispatch::SigLeftButtonPress, handler,&XInteractionEventHandler::LeftButtonPressEvent);
	xsig::connect(eventDispatcher, &XRenderWindowEventDispatch::SigLeftButtonRelease, handler,&XInteractionEventHandler::LeftButtonReleaseEvent);
	xsig::connect(eventDispatcher, &XRenderWindowEventDispatch::SigMiddleButtonPress, handler,&XInteractionEventHandler::MiddleButtonPressEvent);
	xsig::connect(eventDispatcher, &XRenderWindowEventDispatch::SigMiddleButtonRelease, handler,&XInteractionEventHandler::MiddleButtonReleaseEvent);
	xsig::connect(eventDispatcher, &XRenderWindowEventDispatch::SigRightButtonPress, handler,&XInteractionEventHandler::RightButtonPressEvent);
	xsig::connect(eventDispatcher, &XRenderWindowEventDispatch::SigRightButtonRelease, handler,&XInteractionEventHandler::RightButtonReleaseEvent);
	xsig::connect(eventDispatcher, &XRenderWindowEventDispatch::SigEnter, handler,&XInteractionEventHandler::EnterEvent);
	xsig::connect(eventDispatcher, &XRenderWindowEventDispatch::SigLeave, handler,&XInteractionEventHandler::LeaveEvent);
	xsig::connect(eventDispatcher, &XRenderWindowEventDispatch::SigFoucsIn, handler,&XInteractionEventHandler::FoucsInEvent);
	xsig::connect(eventDispatcher, &XRenderWindowEventDispatch::SigFoucsOut, handler,&XInteractionEventHandler::FoucsOutEvent);
	xsig::connect(eventDispatcher, &XRenderWindowEventDispatch::SigResize, handler,&XInteractionEventHandler::ResizeEvent);
	xsig::connect(eventDispatcher, &XRenderWindowEventDispatch::SigKeyPress, handler,&XInteractionEventHandler::KeyPressEvent);
	xsig::connect(eventDispatcher, &XRenderWindowEventDispatch::SigKeyRelease, handler,&XInteractionEventHandler::KeyReleaseEvent);
	xsig::connect(eventDispatcher, &XRenderWindowEventDispatch::SigMouseMove, handler,&XInteractionEventHandler::MouseMoveEvent);
	xsig::connect(eventDispatcher, &XRenderWindowEventDispatch::SigMouseWheelForward, handler,&XInteractionEventHandler::MouseWheelForwardEvent);
	xsig::connect(eventDispatcher, &XRenderWindowEventDispatch::SigMouseWheelBackward, handler, &XInteractionEventHandler::MouseWheelBackwardEvent);
	xsig::connect(eventDispatcher, &XRenderWindowEventDispatch::SigTimeOut, handler, &XInteractionEventHandler::TimeEvent);
}

void XRender::addRenderNode3D(sptr<XGeometryNode>s)
{
	s->setShaderManger(getRenderWindow()->getShaderManger());
	mData->m_actor3DList.push_back(s);
}

void XRender::addRenderNode2D(sptr<XGraphicsItem>)
{
}

void XRender::fitView()
{
	auto boundbox = computeBoundBox();
	getCamera()->resetCamera(boundbox);
}

XQ::Vec2i XRender::window2render(const XQ::Vec2i& windowPos)
{
	auto v =getConvertViewPort();
	return XQ::Vec2i(windowPos[0] - v[0], windowPos[1] - v[1]);
}

XQ::Vec3f XRender::render2window(const XQ::Vec2i& renderPos)
{
	auto v = getConvertViewPort();
	return XQ::Vec3f(renderPos[0] + v[0], renderPos[1] + v[1], 0);
}

XQ::Recti XRender::getConvertViewPort() const
{
	auto renderWindow = getRenderWindow();
	auto w = renderWindow->getWindowWidth();
	auto h = renderWindow->getWindowHeight();
	auto rect = getViewPort();
	auto x = rect.x() * w;
	auto y = rect.y() * h;
	auto w2 = rect.z() * w;
	auto h2 = rect.w() * h;

	return XQ::Recti(x,y,w2,h2);
}

bool XRender::isBelongToRender(const XQ::Vec2i& windowPos) const
{
	auto x = windowPos[0];
	auto y = windowPos[1];

	auto port = getConvertViewPort();
	if (
		(port[0] < x && x < (port[0] + port[2])) &&
		(port[1] < y && y < (port[1] + port[3]))
		)
	{
		return true;
	}
	return false;
}

void XRender::updateViewPort()
{
	XQ::Recti rect = getConvertViewPort();
	
	auto enable = makeShareDbObject<XOpenGLEnable>();
	enable->enable(XOpenGLEnable::EnableType::SCISSOR_TEST);
	
	auto oldViewport = XOpenGLFuntion::xglViewport(rect);
	auto oldScissor = XOpenGLFuntion::xglglScissor(rect);

	getCamera()->setAspect(rect[2]/(double)rect[3]);

	auto c = getBackGroundColor1();
	auto r = c.r2();
	auto g = c.g2();
	auto b = c.b2();
	auto a = c.a2();

	XOpenGLFuntion::xglClearColor(r, g, b, a);
	XOpenGLFuntion::xglClear((unsigned int)XOpenGL::BufferBits::color_buffer_bit);

	//XOpenGLFuntion::xglViewport(oldViewport);
	XOpenGLFuntion::xglglScissor(oldScissor);
	enable->restore();
}

void XRender::updateUbo()
{
	mData->writeUbo();
}

sptr<XInteractionEventHandler> XRender::getOrCreateMultiModeEventHandler()
{
	if (!mData->m_multiModeEventHandler) {
		auto h = makeShareDbObject<XRenderMultiModeInteractionHandler>();
		h->setRender(asDerived<XRender>());
		mData->m_multiModeEventHandler = h;
	}
	return mData->m_multiModeEventHandler;
}

XQ::BoundBox  XRender::computeBoundBox() {
	constexpr double limitMax = std::numeric_limits<double>::max();
	constexpr double limitMin = std::numeric_limits<double>::lowest();;
	XQ::BoundBox boundBox{ limitMax ,limitMax ,limitMax ,limitMin,limitMin,limitMin };
	for (auto& shape : mData->m_actor3DList) {
		auto shapeBoundBox = shape->getBoundBox();
		boundBox.xmin = std::min(boundBox.xmin, shapeBoundBox.xmin);
		boundBox.xmax = std::max(boundBox.xmax, shapeBoundBox.xmax);
		boundBox.ymin = std::min(boundBox.ymin, shapeBoundBox.ymin);
		boundBox.ymax = std::max(boundBox.ymax, shapeBoundBox.ymax);
		boundBox.zmin = std::min(boundBox.zmin, shapeBoundBox.zmin);
		boundBox.zmax = std::max(boundBox.zmax, shapeBoundBox.zmax);
	}

	if (mData->m_actor3DList.size() == 0) {
		boundBox.xmin = -1;
		boundBox.xmax = 1;
		boundBox.ymin = -1;
		boundBox.ymax = 1;
		boundBox.zmin = -1;
		boundBox.zmax = 1;
	}
	return boundBox;
}