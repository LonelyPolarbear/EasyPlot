#include "XRender.h"
#include "XOpenGLRenderWindow.h"
#include "XRenderCamera.h"
#include "XRenderMultiModeInteractionHandler.h"
#include "CameraNavigationHandler.h"
#include "XRenderPickHandler.h"
#include "XManipulatorHandler.h"
#include "XRenderWindowEventDispatch.h"
#include "xsignal/XSignal.h"
#include "lib04_opengl/XOpenGLBuffer.h"
#include "lib04_opengl/XOpenGLEnable.h"
#include "lib04_opengl/XOpenGLFuntion.h"
#include <xrendernode/XGeometryNode.h>
#include <xrendernode/datasource/xCustomSource.h>
#include <xrendernode/XGraphicsItem.h>
#include <lib02_camera/xcamera.h>
#include <xrendernode/renderNode3d/XInfinitePlaneRenderNode.h>
#include <xrendernode/renderNode3d/XGroupRenderNode3d.h>
#include <xrendernode/renderNode3d/XFullScreenQuadNode.h>

#include "XOpenGLRenderWindow.h"
#include "base/xbaserender/baseRender/XBaseRenderWindow.h"


struct XRender::Internal {
	Internal(XRender *render) {
		m_camera = makeShareDbObject<XRenderCamera>();
		host = render;
	}

	sptr<RenderWindowUbo> getUbo() {
		return host->getRenderWindow()->asDerived<XOpenGLRenderWindow>()->getRenderWindowUbo();
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

		getUbo()->writeCamera((int)camera->getProjectionType(),camera->getNear(),m_camera->getFar());
	}

public:
	XRender* host;
	wptr<XOpenGLRenderWindow> m_renderWindow;
	sptr<XRenderMultiModeInteractionHandler> m_multiModeEventHandler;
	sptr<XRenderCamera> m_camera;

	std::vector<sptr<XGraphicsItem>> m_actor2DList;

	XQ::Vec2f m_mousePos;																								//鼠标在窗口中的位置，未做变换
	//std::vector<sptr<XGeometryNode>> m_InfinitePlaneNode;									//无限网格平面

	sptr<XFullScreenQuadNode> m_fullScreenQuadNode;											//全屏四边形，用于背景设置

	xsig::xconnector connector;

	~Internal() {
		connector.disconnect();
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
	XRenderPort::Init();
	XQ_ATTR_ADD_INIT(AttrActive, false);
	XQ_XDATA_ADD(m_group3D);
	XQ::XColor bot_color = AttrBottomColor->getValue();
	XQ::XColor top_color = AttrTopColor->getValue();


	auto handler =getOrCreateMultiModeEventHandler();
	mData->m_fullScreenQuadNode = makeShareDbObject<XFullScreenQuadNode>();
	mData->m_fullScreenQuadNode->setVertexColor({ bot_color ,bot_color ,top_color,top_color });
	mData->m_fullScreenQuadNode->setFarRect();

	//信号的绑定
	mData->connector.connect(AttrBottomColor,&XAttr_Color::sigAttrChanged,[this](sptr<XDataAttribute>, XDataChangeType type){
		if (type == XDataChangeType::ItemDataModified) {
			XQ::XColor bot_color = AttrBottomColor->getValue();
			XQ::XColor top_color = AttrTopColor->getValue();
			mData->m_fullScreenQuadNode->setVertexColor({ bot_color ,bot_color ,top_color,top_color });
		}
	});

	mData->connector.connect(AttrTopColor, &XAttr_Color::sigAttrChanged, [this](sptr<XDataAttribute>, XDataChangeType type) {
		if (type == XDataChangeType::ItemDataModified) {
			XQ::XColor bot_color = AttrBottomColor->getValue();
			XQ::XColor top_color = AttrTopColor->getValue();
			mData->m_fullScreenQuadNode->setVertexColor({ bot_color ,bot_color ,top_color,top_color });
		}
		});
}

void XRender::setRenderWindow(sptr<XBaseRenderWindow> renderWindow)
{
	mData->m_renderWindow = renderWindow->asDerived<XOpenGLRenderWindow>();
	m_group3D->setShaderManger(getRenderWindow()->getShaderManger());
	mData->m_fullScreenQuadNode->setShaderManger(getRenderWindow()->getShaderManger());
}

sptr<XBaseRenderWindow> XRender::getRenderWindow() const
{
	return mData->m_renderWindow.lock();
}

void XRender::setCamera(sptr<XBaseRenderCamera> camera)
{
	mData->m_camera = camera->asDerived<XRenderCamera>();
}

sptr<XBaseRenderCamera> XRender::getCamera() const
{
	return mData->m_camera;
}

void XRender::render(bool isNormal)
{
	//renderwindow确保opengl上下文有效，因此此处可以不对makeCurrent返回值判断
	//所以所有actor需要初始化opengl的资源的接口都可以在此处完成
	if(!makeCurrent())
		return;
	
	//!
	//! 
	updateViewPort(isNormal);

	updateUbo();

	//!
	//! 渲染
	auto enable = makeShareDbObject<XOpenGLEnable>();
	enable->save();
	enable->enable(XOpenGLEnable::EnableType::DEPTH_TEST);
	enable->enable(XOpenGLEnable::EnableType::BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	enable->enable(XOpenGLEnable::EnableType::MULTISAMPLE);
	//enable->save();
	if(!isNormal)
		enable->disable(XOpenGLEnable::EnableType::MULTISAMPLE);
	XOpenGLFuntion::xglClearDepth(1);
	XOpenGLFuntion::xglClear((unsigned int)XOpenGL::BufferBits::depth_buffer_bit);

	
	m_group3D->draw(asDerived<XBaseRender>(), Eigen::Matrix4f::Identity(), isNormal);
	mData->m_fullScreenQuadNode->draw(asDerived<XBaseRender>(),Eigen::Matrix4f::Identity(),isNormal);

	enable->restore();

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
	bool tmp = AttrActive->getValue();
	AttrActive->setValue(isActive);
	return tmp;
}

bool XRender::isActive() const
{
	return AttrActive->getValue();
}

void XRender::setCameraNavigationHandler(sptr<XInteractionEventHandler> interactionEventHandler)
{
	auto multiHandler =getOrCreateMultiModeEventHandler()->asDerived<XRenderMultiModeInteractionHandler>();
	if (multiHandler) {
		multiHandler->setCameraNavigationHandler(interactionEventHandler->asDerived<CameraNavigationHandler>());
	}
}

void XRender::setPickHandler(sptr<XInteractionEventHandler> pickEventHandler)
{
	auto multiHandler = getOrCreateMultiModeEventHandler()->asDerived<XRenderMultiModeInteractionHandler>();
	if (multiHandler) {
		multiHandler->setPickHandler(pickEventHandler->asDerived<XRenderPickHandler>());
	}
}

void XRender::setManipulatorHandler(sptr<XBaseInteractionEventHandler> manipulatorHandler)
{
	auto multiHandler = getOrCreateMultiModeEventHandler()->asDerived<XRenderMultiModeInteractionHandler>();
	if (multiHandler) {
		multiHandler->setManipulatorHandler(manipulatorHandler->asDerived<XManipulatorHandler>());
	}
}

sptr<XInteractionEventHandler> XRender::getCameraNavigationHandler()
{
	auto multiHandler = getOrCreateMultiModeEventHandler()->asDerived<XRenderMultiModeInteractionHandler>();
	if (multiHandler) {
		return multiHandler->getCameraNavigationHandler();
	}
}

sptr<XInteractionEventHandler> XRender::getPickHandler()
{
	auto multiHandler = getOrCreateMultiModeEventHandler()->asDerived<XRenderMultiModeInteractionHandler>();
	if (multiHandler) {
		return multiHandler->getPickHandler();
	}
}

sptr<XBaseInteractionEventHandler> XRender::getManipulatorHandler()
{
	auto multiHandler = getOrCreateMultiModeEventHandler()->asDerived<XRenderMultiModeInteractionHandler>();
	if (multiHandler) {
		return multiHandler->getManipulatorHandler();
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

	mData->connector.connect(eventDispatcher, &XRenderWindowEventDispatch::SigLeftButtonPress, handler, &XInteractionEventHandler::SigLeftButtonPress);
	mData->connector.connect(eventDispatcher, &XRenderWindowEventDispatch::SigLeftButtonRelease, handler, &XInteractionEventHandler::SigLeftButtonRelease);
	mData->connector.connect(eventDispatcher, &XRenderWindowEventDispatch::SigMiddleButtonPress, handler, &XInteractionEventHandler::SigMiddleButtonPress);
	mData->connector.connect(eventDispatcher, &XRenderWindowEventDispatch::SigMiddleButtonRelease, handler, &XInteractionEventHandler::SigMiddleButtonRelease);
	mData->connector.connect(eventDispatcher, &XRenderWindowEventDispatch::SigRightButtonPress, handler, &XInteractionEventHandler::SigRightButtonPress);
	mData->connector.connect(eventDispatcher, &XRenderWindowEventDispatch::SigRightButtonRelease, handler, &XInteractionEventHandler::SigRightButtonRelease);
	mData->connector.connect(eventDispatcher, &XRenderWindowEventDispatch::SigEnter, handler, &XInteractionEventHandler::SigEnter);
	mData->connector.connect(eventDispatcher, &XRenderWindowEventDispatch::SigLeave, handler, &XInteractionEventHandler::SigLeave);
	mData->connector.connect(eventDispatcher, &XRenderWindowEventDispatch::SigFoucsIn, handler, &XInteractionEventHandler::SigFoucsIn);
	mData->connector.connect(eventDispatcher, &XRenderWindowEventDispatch::SigFoucsOut, handler, &XInteractionEventHandler::SigFoucsOut);
	mData->connector.connect(eventDispatcher, &XRenderWindowEventDispatch::SigResize, handler, &XInteractionEventHandler::SigResize);
	mData->connector.connect(eventDispatcher, &XRenderWindowEventDispatch::SigKeyPress, handler, &XInteractionEventHandler::SigKeyPress);
	mData->connector.connect(eventDispatcher, &XRenderWindowEventDispatch::SigKeyRelease, handler, &XInteractionEventHandler::SigKeyRelease);
	mData->connector.connect(eventDispatcher, &XRenderWindowEventDispatch::SigMouseMove, handler, &XInteractionEventHandler::SigMouseMove);
	mData->connector.connect(eventDispatcher, &XRenderWindowEventDispatch::SigMouseWheelForward, handler, &XInteractionEventHandler::SigMouseWheelForward);
	mData->connector.connect(eventDispatcher, &XRenderWindowEventDispatch::SigMouseWheelBackward, handler, &XInteractionEventHandler::SigMouseWheelBackward);
	mData->connector.connect(eventDispatcher, &XRenderWindowEventDispatch::SigTimeOut, handler, &XInteractionEventHandler::SigTimeOut);

	mData->connector.connect(eventDispatcher, &XRenderWindowEventDispatch::SigUserEvent, this, &XRender::SigUserEvent);
	mData->connector.connect(eventDispatcher, &XRenderWindowEventDispatch::SigPredefineEvent, this, &XRender::SigPredefineEvent);
}

void XRender::addRenderNode3D(sptr<XBaseRenderNode>s)
{
	s->setShaderManger(getRenderWindow()->getShaderManger());
	m_group3D->addChildRenderNode(s);
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


void XRender::setInteractMode(XQ::InteractMode mode)
{
	auto handler =getOrCreateMultiModeEventHandler();
	if(auto h=handler->asDerived<XRenderMultiModeInteractionHandler>())
		h->setMode(mode);
}

XQ::InteractMode XRender::getInteractMode() const
{
	if (mData->m_multiModeEventHandler) {
		return mData->m_multiModeEventHandler->getMode();
	}
	return XQ::InteractMode::none;
}

sptr<XBaseRenderNode> XRender::getRenderNode3D(int id)
{
	return m_group3D->findNodeById(id);
}

void XRender::updateViewPort(bool isNormal)
{
	XQ::Recti rect = getConvertViewPort();
	
	auto enable = makeShareDbObject<XOpenGLEnable>();
	enable->save();
	enable->enable(XOpenGLEnable::EnableType::SCISSOR_TEST);
	
	auto oldViewport = XOpenGLFuntion::xglViewport(rect);
	auto oldScissor = XOpenGLFuntion::xglglScissor(rect);

	getCamera()->setAspect(rect[2]/(double)rect[3]);

	auto c = getBackGroundColorTop();
	auto r = c.r2();
	auto g = c.g2();
	auto b = c.b2();
	auto a = c.a2();

	XOpenGLFuntion::xglClear((unsigned int)XOpenGL::BufferBits::color_buffer_bit);
#if 0
	GLuint clearValue[4] = { 0, 0, 0, 0 };
	// 清除颜色缓冲（使用无符号整数清除函数）
	glClearBufferuiv(GL_COLOR, 0, clearValue);
#else
	if (isNormal)
		XOpenGLFuntion::xglClearColor(r, g, b, a);
	else {
		GLuint clearValue[4] = { 0, 0, 0, 0 };
		// 清除颜色缓冲（使用无符号整数清除函数）
		glClearBufferuiv(GL_COLOR, 0, clearValue);
	}
#endif
	
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
	auto shapeBoundBox = m_group3D->getBoundBox(Eigen::Matrix4f::Identity());

	boundBox.xmin = std::min(boundBox.xmin, shapeBoundBox.xmin);
	boundBox.xmax = std::max(boundBox.xmax, shapeBoundBox.xmax);
	boundBox.ymin = std::min(boundBox.ymin, shapeBoundBox.ymin);
	boundBox.ymax = std::max(boundBox.ymax, shapeBoundBox.ymax);
	boundBox.zmin = std::min(boundBox.zmin, shapeBoundBox.zmin);
	boundBox.zmax = std::max(boundBox.zmax, shapeBoundBox.zmax);

	return boundBox;
}