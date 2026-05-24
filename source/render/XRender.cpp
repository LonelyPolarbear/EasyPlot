#include "XRender.h"
#include "XOpenGLRenderWindow.h"
#include "XRenderCamera.h"
#include "XRenderMultiModeInteractionHandler.h"
#include "CameraNavigationHandler.h"
#include "XRenderPickHandler.h"
#include "XManipulatorHandler.h"
#include "XRenderWindowEventDispatch.h"
#include "XDrawManger.h"
#include "xsignal/XSignal.h"
#include "lib04_opengl/XOpenGLBuffer.h"
#include "lib04_opengl/XOpenGLEnable.h"
#include "lib04_opengl/XOpenGLFuntion.h"
#include "lib04_opengl/XOpenGLFramebufferObject.h"
#include <xrendernode/XGeometryNode.h>
#include <xrendernode/datasource/xCustomSource.h>
#include <xrendernode/XGraphicsItem.h>
#include <lib02_camera/xcamera.h>
#include <xrendernode/renderNode3d/XInfinitePlaneRenderNode.h>
#include <xrendernode/renderNode3d/XGroupRenderNode3d.h>
#include <xrendernode/renderNode3d/XFullScreenQuadNode.h>
#include <xrendernode/renderNode3d/XSmaaFullScreenQuadNode.h>
#include <xrendernode/datasource/xchamferCubeSource.h>
#include <xrendernode/datasource/xcubeSource.h>

#include "XOpenGLRenderWindow.h"
#include "base/xbaserender/baseRender/XBaseRenderWindow.h"
#include "base/xbaserender/baseNode/XBaseRenderTexture.h"

#include "lib03_stbImage/stbImage.h"
#include "xlog/XLogger.h"
#include "xalgo/XAlgo.h"


struct XRender::Internal {
	Internal(XRender *render) {
		m_camera = makeShareDbObject<XRenderCamera>();
		m_drawManger = makeShareDbObject<XDrawManger>();
		host = render;
	}

	sptr<RenderWindowUbo> getUbo() {
		return host->getRenderWindow()->asDerived<XOpenGLRenderWindow>()->getRenderWindowUbo();
	}

	void writeUbo() {
		//!
		//! 
		auto camera = m_camera;
		getUbo()->writeVS_globalCamera( camera->getViewMatrix(), camera->projectionMatrix());

		//!
		//! 
		auto view_port = host->getConvertViewPort();
		auto renderPos = m_mousePos;
		renderPos[0] -= view_port[0];
		renderPos[1] -= view_port[1];
		getUbo()->writeFS(XQ::Vec2f(view_port[2],view_port[3]), renderPos);		//视口宽高和鼠标位置

		getUbo()->writeCamera((int)camera->getProjectionType(),camera->getNear(),m_camera->getFar());
	}

	void writeDynamicCamera(const Eigen::Matrix4f& viewMatrix, const Eigen::Matrix4f& projMatrix) {
		getUbo()->writeVS_dynamicCamera(viewMatrix, projMatrix);
	}

	void SlotRenWindowResize(XQ::Vec2i size) {
		host->makeCurrent();
		m_drawManger->SlotRenderSizeChanged(size);
		host->doneCurrent();
	}

	sptr<XBaseRenderNode> AddAxisNode() {
		sptr<XGeometryNode> cubeNode = makeShareDbObject<XGeometryNode>();
		cubeNode->setName("Axis");
		int cube_w = 18;
		int cube_h = 18;
		int cube_z = 18;
		sptr<xchamferCubeSource> cubesource = makeShareDbObject<xchamferCubeSource>();
		cubeNode->Attribute->AttrSizePolicy->AttrPositionType->setValue(XRenderNodeOriginPositionType::free);
		cubeNode->Attribute->AttrSizePolicy->AttrPositionOrien->setValue(XRenderNodeOriginPositionOrien::left_bottom);
		cubeNode->Attribute->AttrSizePolicy->AttrPositionPos->setValue(XQ::Vec2i(28, 28));
		cubeNode->Attribute->AttrUseNormalCamera->setValue(false);

		//cubeNode->Attribute->AttrSizePolicy->AttrIsFixedSize->setValue(false);
		//cubeNode->Attribute->AttrSizePolicy->AttrFixedPixel->setValue(XQ::Vec3i(15, 15, 15));
		//cubeNode->Attribute->AttrUseNormalCamera->setValue(false);
		cubeNode->setInput(cubesource);
		cubeNode->setSingleColor(XQ::Vec4f(0, 0, 0, 1));
		cubeNode->setSelectedColor(XQ::Vec4f(1, 0, 0, 1));
		cubeNode->setPolygonMode(PolygonMode::all);
		cubeNode->setColorMode(ColorMode::FaceColor);
		cubeNode->scale(cube_w, cube_h, cube_z);
		cubeNode->State->AttrHasSelect->setValue(true);
		cubesource->Modified();
		host->mScreenNodes->addChildRenderNode(cubeNode);
		return cubeNode;
	}

	sptr<XBaseRenderNode>  AddCubeTestNode() {
		//默认添加一个立方体节点
		sptr<XGeometryNode> cubeNode = makeShareDbObject<XGeometryNode>();
		cubeNode->setName("cube");
		sptr<XCubeSource> cubesource = makeShareDbObject<XCubeSource>();
		cubeNode->setInput(cubesource);
		cubeNode->setSingleColor(XQ::Vec4f(0, 0, 0, 1));
		cubeNode->setSelectedColor(XQ::Vec4f(1, 0, 0, 1));
		cubeNode->setPolygonMode(PolygonMode::all);
		cubeNode->setColorMode(ColorMode::FaceColor);
		cubeNode->scale(10, 10, 10);
		cubeNode->translate(0, 1, 0);
		cubeNode->State->AttrHasSelect->setValue(true);
		cubesource->Modified();
		host->mMainFboNodes->addChildRenderNode(cubeNode);
		return cubeNode;
	}

	sptr<XBaseRenderNode>  AddInfinitePlaneNode() {
		sptr<XInfinitePlaneRenderNode> InfinitePlaneNode = makeShareDbObject<XInfinitePlaneRenderNode>();
		InfinitePlaneNode->setName("grid");
		host->addRenderNode3D(InfinitePlaneNode);		//网格节点应该最后绘制，才能避免遮挡问题	//TODO

		host->getCamera()->sigDataChanged.connect([InfinitePlaneNode](sptr<XDataObject> camera, XDataChangeType type) {
			if (type == XDataChangeType::DataModified) {
				auto c = camera->asDerived<XRenderCamera>();
				if (c) {
					auto ss = c->getFrustumInWorld();
					auto intersections = XQ::XAlgo::getFrustumXOZIntersections(ss.data(), ss.data() + 4);
					InfinitePlaneNode->setRect({ intersections[0],intersections[1] ,intersections[2] ,intersections[3] });
				}
			}
			});
		return InfinitePlaneNode;
	}

public:
	XRender* host;
	wptr<XOpenGLRenderWindow> m_renderWindow;
	sptr<XRenderMultiModeInteractionHandler> m_multiModeEventHandler;
	sptr<XRenderCamera> m_camera;
	sptr<XDrawManger> m_drawManger;
	xsig::xconnector connector;

	std::vector<sptr<XGraphicsItem>> m_actor2DList;

	XQ::Vec2f m_mousePos;																										///< 鼠标在窗口中的位置，未做变换

	sptr<XFullScreenQuadNode> m_BackGroundNode;															///< 全屏四边形，用于背景设置
	sptr<XSmaaFullScreenQuadNode> m_PostSmaaNode;														///< 全屏四边形，用于后处理，将G-buffer的颜色附件作为输入，输出到屏幕上

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
	XQ_ATTR_ADD_INIT(AttrSmaa, false);
	XQ_ATTR_ADD_INIT(AttrPostProcess,true);
	XQ_XDATA_ADD(mMainFboNodes);
	XQ_XDATA_ADD(mScreenNodes);

	XQ::XColor bot_color = AttrBottomColor->getValue();
	XQ::XColor top_color = AttrTopColor->getValue();

	mData->m_drawManger->setRender(asDerived<XBaseRender>());

	auto handler =getOrCreateMultiModeEventHandler();

	mData->m_PostSmaaNode = makeShareDbObject<XSmaaFullScreenQuadNode>();
	mData->m_PostSmaaNode->setColorMode(ColorMode::textureColor);
	mData->m_PostSmaaNode->setNearRect();


	mData->m_BackGroundNode = makeShareDbObject<XFullScreenQuadNode>();
	mData->m_BackGroundNode->setVertexColor({ bot_color ,bot_color ,top_color,top_color });
	mData->m_BackGroundNode->setFarRect();

	//坐标轴节点
	//mData->AddAxisNode();

	//mData->AddCubeTestNode();

	//mData->AddInfinitePlaneNode();

	//信号的绑定
	mData->connector.connect(AttrBottomColor,&XAttr_Color::sigAttrChanged,[this](sptr<XDataAttribute>, XDataChangeType type){
		if (type == XDataChangeType::ItemDataModified) {
			XQ::XColor bot_color = AttrBottomColor->getValue();
			XQ::XColor top_color = AttrTopColor->getValue();
			mData->m_BackGroundNode->setVertexColor({ bot_color ,bot_color ,top_color,top_color });
		}
	});

	mData->connector.connect(AttrTopColor, &XAttr_Color::sigAttrChanged, [this](sptr<XDataAttribute>, XDataChangeType type) {
		if (type == XDataChangeType::ItemDataModified) {
			XQ::XColor bot_color = AttrBottomColor->getValue();
			XQ::XColor top_color = AttrTopColor->getValue();
			mData->m_BackGroundNode->setVertexColor({ bot_color ,bot_color ,top_color,top_color });
		}
		});
}

void XRender::setRenderWindow(sptr<XBaseRenderWindow> renderWindow)
{
	mData->m_renderWindow = renderWindow->asDerived<XOpenGLRenderWindow>();
	mMainFboNodes->setShaderManger(getRenderWindow()->getShaderManger());
	mData->m_BackGroundNode->setShaderManger(getRenderWindow()->getShaderManger());
	mScreenNodes->setShaderManger(getRenderWindow()->getShaderManger());
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

sptr<XDataBaseObject> XRender::getMainRenderTarget()
{
	makeCurrent();
	mData->m_drawManger->biltScreen();
	doneCurrent();
	return mData->m_drawManger->getBiltFbo();
}


sptr<XDataBaseObject> XRender::getOverlayRenderTarget(int lay)
{
	makeCurrent();
	mData->m_drawManger->biltOverlay(lay);
	doneCurrent();
	return mData->m_drawManger->getBiltFbo();
}

void XRender::render() {
	if (!makeCurrent())
		return;

	updateUbo();

	renderGBuffer();

	renderToScreen();

	doneCurrent();
}

void XRender::renderGBuffer()
{
	{
		mData->m_drawManger->InitRenderSource();

		auto viewPort = this->getConvertViewPort();
		XOpenGLFuntion::xglglScissor(XQ::Recti(0, 0, viewPort[2], viewPort[3]));
		XOpenGLFuntion::xglViewport(XQ::Recti(0, 0, viewPort[2], viewPort[3]));
		getCamera()->setAspect(viewPort[2] / (double)viewPort[3]);

		auto fbo = mData->m_drawManger->getScreenFbo();
		if (fbo) {
			auto screenFbo = fbo->asDerived<XOpenGLFramebufferObject>();
			screenFbo->bind();
			GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
			glDrawBuffers(2, drawBuffers);
		}

		XOpenGLFuntion::xglClearColor(XQ::Vec4f(0, 0, 0, 1), 0);
		XOpenGLFuntion::xglClearColor(XQ::Vec4u(0, 0, 0, 0), 1);
		XOpenGLFuntion::xglClearDepthStencil(1, 0);

		auto enable = makeShareDbObject<XOpenGLEnable>();
		enable->save();
		enable->enable(XOpenGLEnable::EnableType::DEPTH_TEST);
		enable->enable(XOpenGLEnable::EnableType::BLEND);
		enable->enable(XOpenGLEnable::EnableType::MULTISAMPLE);

		//绘制背景
		auto tmp = XOpenGLFuntion::xglDepthFunc(XOpenGL::DepthOrStencilCompFunType::XGL_LEQUAL);
		mData->m_BackGroundNode->draw(asDerived<XBaseRender>(), Eigen::Matrix4f::Identity());
		XOpenGLFuntion::xglDepthFunc(tmp);

		mMainFboNodes->draw(asDerived<XBaseRender>(), Eigen::Matrix4f::Identity());

		enable->restore();
		if (fbo) {
			fbo->asDerived<XOpenGLFramebufferObject>()->release();
		}
	}

	renderScreen3dNode2Buffer();
}

void XRender::renderScreen3dNode2Buffer()
{
		//坐标轴的渲染，始终显示在最前面，同时允许拾取
		auto layFbo = mData->m_drawManger->getOverlayFbo(0);
		if (layFbo) {
			{
				//先清除缓冲
				auto fbo = layFbo->asDerived<XOpenGLFramebufferObject>();
				fbo->bind();
				auto enable = makeShareDbObject<XOpenGLEnable>();
				enable->save();
				auto viewPort = getConvertViewPort();
				XOpenGLFuntion::xglglScissor(viewPort);
				XOpenGLFuntion::xglViewport(viewPort);

				XOpenGLFuntion::xglClearColor(XQ::Vec4f(0, 0, 0, 0), 0);		//alpha设置为0，方便混合
				XOpenGLFuntion::xglClearColor(XQ::Vec4u(0, 0, 0, 0), 1);
				XOpenGLFuntion::xglClearDepthStencil(1, 0);
				enable->restore();
				fbo->release();
			}
			auto num = mScreenNodes->getChildRenderNodeCount();
			for (int i = 0; i < num; i++) {
				int screen_width = 50;		//todo 待优化，暂不处理了
				int screen_height = 50;
				auto projectMatrix = getCamera()->orthoMatrix(screen_width, screen_height, 0.1, 5000);
				auto cameraMat = getCamera()->getViewMatrix().inverse();
				//提取旋转部分
				auto rotMat = XQ::Matrix::extractRotateMat(cameraMat);

				auto newCamertMat = XQ::Matrix::convert(rotMat) * XQ::Matrix::transltae(0, 0, 200);
				auto newViewMatrix = newCamertMat.inverse().matrix();

				mData->writeDynamicCamera(newViewMatrix, projectMatrix);

				auto fbo = layFbo->asDerived<XOpenGLFramebufferObject>();
				fbo->bind();
				{
					GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
					glDrawBuffers(2, drawBuffers);
				}
				auto enable = makeShareDbObject<XOpenGLEnable>();
				enable->save();
				enable->enable(XOpenGLEnable::EnableType::DEPTH_TEST);
				auto viewPort = this->getConvertViewPort();
				XOpenGLFuntion::xglglScissor(XQ::Recti(0, 0, screen_width, screen_height));
				XOpenGLFuntion::xglViewport(XQ::Recti(0, 0, screen_width, screen_height));

				XOpenGLFuntion::xglClearColor(XQ::Vec4f(0, 0, 0, 0), 0);		//alpha设置为0，方便混合
				XOpenGLFuntion::xglClearColor(XQ::Vec4u(0, 0, 0, 0), 1);
				XOpenGLFuntion::xglClearDepthStencil(1, 0);
				
				auto node = mScreenNodes->getChildRenderNode(i);
				node->draw(asDerived<XBaseRender>(), Eigen::Matrix4f::Identity());

				enable->restore();
				fbo->release();
			}
		}
}

void XRender::renderToScreen()
{
	if(!AttrPostProcess->getValue())
		return;
	
	
	XQ::Recti rect = getConvertViewPort();

	auto enable = makeShareDbObject<XOpenGLEnable>();
	enable->save();
	enable->enable(XOpenGLEnable::EnableType::SCISSOR_TEST);

	XOpenGLFuntion::xglglScissor(rect);
	XOpenGLFuntion::xglViewport(rect);
	getCamera()->setAspect(rect[2] / (double)rect[3]);

	XOpenGLFuntion::xglClearColor(XQ::Vec4f(0, 0, 0, 1), 0);
	XOpenGLFuntion::xglClearDepthStencil(1, 0);

	{
		auto fbo = mData->m_drawManger->getScreenFbo();
		auto screenFbo = fbo->asDerived<XOpenGLFramebufferObject>();
		mData->m_PostSmaaNode->setInputColorTexture(screenFbo->getColorAttachment(0), screenFbo->getDepthAttachment());
	}
	
	mData->m_PostSmaaNode->AttrEnableSmaa->setValue(AttrSmaa->getValue());

	enable->disable(XOpenGLEnable::EnableType::DEPTH_TEST);
	enable->enable(XOpenGLEnable::EnableType::MULTISAMPLE);
	enable->disable(XOpenGLEnable::EnableType::BLEND);
	mData->m_PostSmaaNode->draw(asDerived<XBaseRender>(), Eigen::Matrix4f::Identity());

	renderScreen3dNodeBuffer2Screen();

	enable->restore();
}

void XRender::renderScreen3dNodeBuffer2Screen()
{
	//渲染坐标轴
	{
		auto tmpEnable = makeShareDbObject<XOpenGLEnable>();
		tmpEnable->save();
		tmpEnable->disable(XOpenGLEnable::EnableType::DEPTH_TEST);
		tmpEnable->enable(XOpenGLEnable::EnableType::BLEND);
		tmpEnable->enable(XOpenGLEnable::EnableType::MULTISAMPLE);
		XOpenGLFuntion::xglBlendFunc(XOpenGL::BlendFuncFactor::XGL_SRC_ALPHA, XOpenGL::BlendFuncFactor::XGL_ONE_MINUS_SRC_ALPHA);
		{
			auto fbo = mData->m_drawManger->getOverlayFbo(0)->asDerived<XOpenGLFramebufferObject>();
			mData->m_PostSmaaNode->setInputColorTexture(fbo->getColorAttachment(0), fbo->getDepthAttachment());
			mData->m_PostSmaaNode->draw(asDerived<XBaseRender>(), Eigen::Matrix4f::Identity());
		}

		tmpEnable->restore();
	}
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

	mData->connector.connect(eventDispatcher, &XRenderWindowEventDispatch::SigLeftButtonDoublePress, handler, &XInteractionEventHandler::SigLeftButtonDoublePress);
	mData->connector.connect(eventDispatcher, &XRenderWindowEventDispatch::SigRightButtonDoublePress, handler, &XInteractionEventHandler::SigRightButtonDoublePress);
	mData->connector.connect(eventDispatcher, &XRenderWindowEventDispatch::SigMiddleButtonDoublePress, handler, &XInteractionEventHandler::SigMiddleButtonDoublePress);

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

	mData->connector.connect(eventDispatcher, &XRenderWindowEventDispatch::SigResize, [this](XQ::Vec2i size){
		mData->SlotRenWindowResize(size);
	});

	mData->connector.connect(eventDispatcher, &XRenderWindowEventDispatch::SigMouseMove, [this](XQ::Vec2i pos, XQ::KeyboardModifier) {
		mData->m_mousePos[0] = pos[0];
		mData->m_mousePos[1] = pos[1];
		});
}

void XRender::addRenderNode3D(sptr<XBaseRenderNode>s)
{
	if (auto w = getRenderWindow()) {
		if (auto m = w->getShaderManger()) {
			s->setShaderManger(getRenderWindow()->getShaderManger());
		}
	}
	mMainFboNodes->addChildRenderNode(s);
}

void XRender::addScreenRenderNode3D(sptr<XBaseRenderNode> s)
{
	if (auto w = getRenderWindow()) {
		if (auto m = w->getShaderManger()) {
			s->setShaderManger(getRenderWindow()->getShaderManger());
		}
	}
	mScreenNodes->addChildRenderNode(s);
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
	return mMainFboNodes->findNodeById(id);
}

sptr<XBaseRenderNode> XRender::getRenderNode3D(int id, int lay)
{
	if(lay ==0)
		return mScreenNodes->findNodeById(id);
	return nullptr;
}

sptr<XBaseDrawManger> XRender::getDrawManger()
{
	return mData->m_drawManger;
}	

sptr<XBaseRenderNode> XRender::AddAxisNode()
{
	return mData->AddAxisNode();
}

sptr<XBaseRenderNode> XRender::AddCubeTestNode()
{
	return mData->AddCubeTestNode();
}

sptr<XBaseRenderNode> XRender::AddInfinitePlaneNode()
{
	return mData->AddInfinitePlaneNode();
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
	auto shapeBoundBox = mMainFboNodes->getBoundBox(Eigen::Matrix4f::Identity());

	boundBox.xmin = std::min(boundBox.xmin, shapeBoundBox.xmin);
	boundBox.xmax = std::max(boundBox.xmax, shapeBoundBox.xmax);
	boundBox.ymin = std::min(boundBox.ymin, shapeBoundBox.ymin);
	boundBox.ymax = std::max(boundBox.ymax, shapeBoundBox.ymax);
	boundBox.zmin = std::min(boundBox.zmin, shapeBoundBox.zmin);
	boundBox.zmax = std::max(boundBox.zmax, shapeBoundBox.zmax);

	return boundBox;
}