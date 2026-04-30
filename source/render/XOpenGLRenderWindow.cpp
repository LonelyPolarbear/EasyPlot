#include "XOpenGLRenderWindow.h"
#include "lib04_opengl/XOpenGLContext.h"
#include "lib04_opengl/XOpenGLBuffer.h"
#include "lib04_opengl/XOpenGLFuntion.h"
#include "XRenderWindowEventDispatch.h"
#include "XRender.h"

RenderWindowUbo::RenderWindowUbo() {}

RenderWindowUbo::~RenderWindowUbo() {}

void RenderWindowUbo::create()
{
	if (init_done)
	{
		return;
	}
	uniformBufferVs = makeShareDbObject<XOpenGLBuffer>();
	uniformBufferVs->setBufferType(XOpenGLBuffer::UniformBuffer);
	uniformBufferVs->setUsagePattern(XOpenGLBuffer::UsagePattern::StaticDraw);
	uniformBufferVs->create();
	uniformBufferVs->bind();
	uniformBufferVs->allocate(sizeof(Eigen::Matrix4f) * 2);
	uniformBufferVs->setBufferBindIdx(1);
	uniformBufferVs->release();

	uniformBufferFs = makeShareDbObject<XOpenGLBuffer>();
	uniformBufferFs->setBufferType(XOpenGLBuffer::UniformBuffer);
	uniformBufferFs->setUsagePattern(XOpenGLBuffer::UsagePattern::StaticDraw);
	uniformBufferFs->create();
	uniformBufferFs->bind();
	uniformBufferFs->allocate(sizeof(Eigen::Vector2f) * 4);
	uniformBufferFs->setBufferBindIdx(2);
	uniformBufferFs->release();

	uniformBufferGs = makeShareDbObject<XOpenGLBuffer>();
	uniformBufferGs->setBufferType(XOpenGLBuffer::UniformBuffer);
	uniformBufferGs->setUsagePattern(XOpenGLBuffer::UsagePattern::StaticDraw);
	uniformBufferGs->create();
	uniformBufferGs->bind();
	uniformBufferGs->allocate(sizeof(Eigen::Vector2f) * 2 + 4);
	uniformBufferGs->setBufferBindIdx(3);
	uniformBufferGs->release();

	uniformBufferVs2d = makeShareDbObject<XOpenGLBuffer>();
	uniformBufferVs2d->setBufferType(XOpenGLBuffer::UniformBuffer);
	uniformBufferVs2d->setUsagePattern(XOpenGLBuffer::UsagePattern::StaticDraw);
	uniformBufferVs2d->create();
	uniformBufferVs2d->bind();
	uniformBufferVs2d->allocate(sizeof(Eigen::Matrix4f) * 2);
	uniformBufferVs2d->setBufferBindIdx(4);
	uniformBufferVs2d->release();

	uniformBufferCamera = makeShareDbObject<XOpenGLBuffer>();
	uniformBufferCamera->setBufferType(XOpenGLBuffer::UniformBuffer);
	uniformBufferCamera->setUsagePattern(XOpenGLBuffer::UsagePattern::StaticDraw);
	uniformBufferCamera->create();
	uniformBufferCamera->bind();
	uniformBufferCamera->allocate(sizeof(int)*4);
	uniformBufferCamera->setBufferBindIdx(5);
	uniformBufferCamera->release();

	init_done = true;
}

void RenderWindowUbo::writeVS(const Eigen::Matrix4f& view, const Eigen::Matrix4f& projection) {
	uniformBufferVs->bind();
	uniformBufferVs->write(0, view.data(), 16);
	uniformBufferVs->write(16, projection.data(), 16);
	uniformBufferVs->release();
}

void RenderWindowUbo::writeVS2d(const Eigen::Matrix4f& virtualWorldFrame, const Eigen::Matrix4f& sceneFrameInVirtualWorld) {
	uniformBufferVs2d->bind();
	uniformBufferVs2d->write(0, virtualWorldFrame.data(), 16);
	uniformBufferVs2d->write(16, sceneFrameInVirtualWorld.data(), 16);
	uniformBufferVs2d->release();
}

void RenderWindowUbo::writeFS(const XQ::Vec2f& viewport, const XQ::Vec2f& mousePos) {
	uniformBufferFs->bind();
	uniformBufferFs->write(0, viewport.ptr(0), 2);
	uniformBufferFs->write(2, mousePos.ptr(0), 2);
	uniformBufferFs->release();
}

void RenderWindowUbo::writeGS(const Eigen::Vector2f& viewport, float sceneScale) {
	uniformBufferGs->bind();
	uniformBufferGs->write(0, viewport.data(), 2);
	uniformBufferGs->write(2, &sceneScale, 1);
	uniformBufferGs->release();
}

void RenderWindowUbo::writeCamera(int type, float near, float far)
{
	uniformBufferCamera->bind();
	uniformBufferCamera->write(0, &type, 1);
	uniformBufferCamera->write(1, &near, 1);
	uniformBufferCamera->write(2, &far, 1); 
	uniformBufferGs->release();
}

XOpenGLRenderWindow::XOpenGLRenderWindow()
{
}

XOpenGLRenderWindow::~XOpenGLRenderWindow()
{
}

void XOpenGLRenderWindow::SetWindowId(uint64_t winId)
{
	if (!m_context->isValid()) {
		if (!m_context->create(winId)) {
			return;
		}

		//opengl上下文已经创建
		m_shaderManger->initGLResource();
		m_renderWindowUbo->create();
	}
}

void XOpenGLRenderWindow::Init()
{
	XDataObject::Init();
	XQ_XDATA_ADD(m_renderList);
    m_context =makeShareDbObject<XOpenGLContext>();
	m_eventDispatch = makeShareDbObject<XRenderWindowEventDispatch>();
	m_eventDispatch->setRenderWindow(asDerived<XOpenGLRenderWindow>());
	m_shaderManger = makeShareDbObject<xShaderManger>();
	m_renderWindowUbo = makeShareDbObject<RenderWindowUbo>();
	bindSigalSlot();
}

void XOpenGLRenderWindow::bindSigalSlot()
{
	xsig::connect(m_eventDispatch, &XRenderWindowEventDispatch::SigResize, this, &XOpenGLRenderWindow::slotUpdateSize);
	xsig::connect(m_eventDispatch, &XRenderWindowEventDispatch::SigMouseMove, [this](XQ::Vec2i windowPos, XQ::KeyboardModifier){
		//根据windowPos判断当前应该激活哪个窗口，这个绑定工作是在最前面的

		for (auto r : *m_renderList) {
			auto ren = r->asDerived<XRender>();
			ren->setActive(ren->isBelongToRender(windowPos));
		}
	});
}

void XOpenGLRenderWindow::render()
{
	//调用渲染之前，需要确保上下文存在
	if (!m_context->isValid()) {
		return;
	}

	makeCurrent();
	XOpenGLFuntion::xglBindFramebuffer(XOpenGL::FrameBufferType::framebuffer,0);
	for (auto r : *m_renderList) {
		auto ren = r->asDerived<XRender>();
		ren->render();
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
}

int XOpenGLRenderWindow::getWindowWidth() const
{
	return m_windowSize.x();
}

int XOpenGLRenderWindow::getWindowHeight() const
{
	return m_windowSize.y();
}

sptr<XBaseRenderWindowEventDispatch> XOpenGLRenderWindow::getEventDispatcher() const
{
	return m_eventDispatch;
}

void XOpenGLRenderWindow::addRender(sptr<XBaseRender> ren)
{
	if(!ren)
		return;
	m_renderList->append(ren);
	ren->setRenderWindow(asDerived<XOpenGLRenderWindow>());

	//同时 将事件分发器分发的事件路由到render的事件处理器上
	ren->connectToRenderWindowSignals();
}

std::vector<sptr<XBaseRender>> XOpenGLRenderWindow::getRenders() const
{
	std::vector<sptr<XBaseRender>> result;
	for (auto r : *m_renderList) {
		result.push_back(r->asDerived<XBaseRender>());
	}
	return result;
}

sptr<xShaderManger> XOpenGLRenderWindow::getShaderManger() const
{
	return m_shaderManger;
}

sptr<RenderWindowUbo> XOpenGLRenderWindow::getRenderWindowUbo() const
{
	return m_renderWindowUbo;
}