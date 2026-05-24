#include "XDrawManger.h"

#include <lib04_opengl/XOpenGLFramebufferObject.h>
#include <lib04_opengl/XOpenGLFuntion.h>

#include <base/xbaserender/baseNode/XBaseRenderTexture.h>
#include <base/xbaserender/baseRender/XBaseRender.h>
#include <base/xbaserender/baseRender/XBaseRenderWindow.h>

#include <glew/glew.h>

class XDrawManger::Internal {
	public:
		wptr<XBaseRender> render;
		sptr<XOpenGLFramebufferObject> fboScreen;
		sptr<XOpenGLFramebufferObject> biltFbo;
		std::map<int, sptr<XOpenGLFramebufferObject>> fboOverlays;		//0层用于轮廓渲染
		int sampleNum =8;

	void initFboScreen();
	void initOverlayFbo(int lay);
	sptr<XBaseRender> getRender(){
		return render.lock();
	}
	void SlotRenderSizeChanged(XQ::Vec2i);

	void biltScreen();
	void biltOverlay(int lay);
protected:
	void bilt(sptr<XOpenGLFramebufferObject> msaaSrc, sptr<XOpenGLFramebufferObject> dest);

	
};

XDrawManger::XDrawManger():mData(new Internal)
{
}

XDrawManger::~XDrawManger()
{
}

sptr<XDataBaseObject> XDrawManger::getScreenFbo()
{
	return mData->fboScreen;
}

sptr<XDataBaseObject> XDrawManger::getBiltFbo()
{
	return mData->biltFbo;
}

sptr<XDataBaseObject> XDrawManger::getOverlayFbo(int lay)
{
	if (mData->fboOverlays.find(lay) != mData->fboOverlays.end()) {
		return mData->fboOverlays[lay];
	}
	return nullptr;
}

void XDrawManger::InitRenderSource()
{
	mData->initFboScreen();
	mData->initOverlayFbo(0);
}

void XDrawManger::setRender(sptr<XBaseRender> base_render)
{
	mData->render = base_render;
}

void XDrawManger::biltScreen()
{
	mData->biltScreen();
}


void XDrawManger::biltOverlay(int lay)
{
	mData->biltOverlay(lay);
}

void XDrawManger::SlotRenderSizeChanged(XQ::Vec2i size)
{
	mData->SlotRenderSizeChanged(size);
}

//---------------------------
void XDrawManger::Internal::initFboScreen()
{
	if (!fboScreen) {
		fboScreen = makeShareDbObject<XOpenGLFramebufferObject>();
		fboScreen->create();
		fboScreen->bind();
		fboScreen->addAttachmentMSAA(XOpenGLFramebufferObject::Attachment::Color, XOpenGLTexture::TextureFormat::RGBA8_UNorm, XOpenGLTexture::PixelFormat::RGBA, XOpenGLTexture::PixelType::UInt8, sampleNum, 0);											//颜色附件
		fboScreen->addAttachmentMSAA(XOpenGLFramebufferObject::Attachment::Color, XOpenGLTexture::TextureFormat::RGBA32U, XOpenGLTexture::PixelFormat::RGBA_Integer, XOpenGLTexture::PixelType::UInt32, sampleNum, 1);										//ID附件
#
		fboScreen->addAttachmentMSAA(XOpenGLFramebufferObject::Attachment::CombinedDepthStencil, XOpenGLTexture::TextureFormat::D24S8, XOpenGLTexture::PixelFormat::DepthStencil, XOpenGLTexture::PixelType::UInt32_D24S8, sampleNum,0);

		bool ss = fboScreen->isComplete();

		auto viewport = getRender()->getConvertViewPort();
		fboScreen->updateBufferSize(viewport[2], viewport[3]);
		fboScreen->release();

		auto tex = makeShareDbObject<XBaseRenderTexture>();
		tex->setTexture(fboScreen->getColorAttachment(0));

		biltFbo = makeShareDbObject<XOpenGLFramebufferObject>();
		biltFbo->create();
		biltFbo->bind();
		biltFbo->addAttachment(XOpenGLFramebufferObject::Attachment::Color, XOpenGLTexture::TextureFormat::RGBA8_UNorm, XOpenGLTexture::PixelFormat::RGBA, XOpenGLTexture::PixelType::UInt8, 0);
		biltFbo->addAttachment(XOpenGLFramebufferObject::Attachment::Color, XOpenGLTexture::TextureFormat::RGBA32U, XOpenGLTexture::PixelFormat::RGBA_Integer, XOpenGLTexture::PixelType::UInt32, 1);
		biltFbo->addAttachment(XOpenGLFramebufferObject::Attachment::CombinedDepthStencil, XOpenGLTexture::TextureFormat::D24S8, XOpenGLTexture::PixelFormat::DepthStencil, XOpenGLTexture::PixelType::UInt32_D24S8, 0);


		bool ss2 = biltFbo->isComplete();

		biltFbo->updateBufferSize(viewport[2], viewport[3]);
		biltFbo->release();
	}
}

void XDrawManger::Internal::initOverlayFbo(int lay)
{
	if(fboOverlays.find(lay) ==fboOverlays.end())
	{
		auto fbo = makeShareDbObject<XOpenGLFramebufferObject>();
		fbo->create();
		fbo->bind();
		fbo->addAttachmentMSAA(XOpenGLFramebufferObject::Attachment::Color, XOpenGLTexture::TextureFormat::RGBA8_UNorm, XOpenGLTexture::PixelFormat::RGBA, XOpenGLTexture::PixelType::UInt8, sampleNum, 0);											//颜色附件
		fbo->addAttachmentMSAA(XOpenGLFramebufferObject::Attachment::Color, XOpenGLTexture::TextureFormat::RGBA32U, XOpenGLTexture::PixelFormat::RGBA_Integer, XOpenGLTexture::PixelType::UInt32, sampleNum, 1);										//ID附件
		fbo->addAttachmentMSAA(XOpenGLFramebufferObject::Attachment::CombinedDepthStencil, XOpenGLTexture::TextureFormat::D24S8, XOpenGLTexture::PixelFormat::DepthStencil, XOpenGLTexture::PixelType::UInt32_D24S8, sampleNum, 0);	//深度模板附件
		bool ss = fbo->isComplete();

		auto ren_window = getRender()->getRenderWindow();
		fbo->updateBufferSize(ren_window->getWindowWidth(), ren_window->getWindowHeight());
		fbo->release();

		fboOverlays[lay] = fbo;
	}
}

void XDrawManger::Internal::biltScreen()
{
	bilt(fboScreen,biltFbo);
}

void XDrawManger::Internal::bilt(sptr<XOpenGLFramebufferObject> msaaSrc, sptr<XOpenGLFramebufferObject> dest)
{
	if (!msaaSrc || !dest) {
		return;
	}
	auto w = msaaSrc->getWidth();
	auto h = msaaSrc->getHeight();
	XOpenGLFuntion::checkGLError();
	msaaSrc->bind(XOpenGL::FrameBufferType::readBuffer);
	glReadBuffer(GL_COLOR_ATTACHMENT0);

	dest->bind(XOpenGL::FrameBufferType::drawBuffer);
	GLenum drawBuffer = GL_COLOR_ATTACHMENT0;							// 指定目标附件为 GL_COLOR_ATTACHMENT2
	glDrawBuffers(1, &drawBuffer);														// 注意：即使单附件也需用数组
	XOpenGLFuntion::xglBlitFramebuffer(0, 0, w, h, 0, 0, w, h, XOpenGL::FlagBits::color_buffer_bit, XOpenGL::FilterType::nearest);

	glReadBuffer(GL_COLOR_ATTACHMENT1);
	drawBuffer = GL_COLOR_ATTACHMENT1;
	glDrawBuffers(1, &drawBuffer);
	XOpenGLFuntion::xglBlitFramebuffer(0, 0, w, h, 0, 0, w, h, XOpenGL::FlagBits::color_buffer_bit, XOpenGL::FilterType::nearest);


	glReadBuffer(GL_NONE);
	glDrawBuffer(GL_NONE);
	XOpenGLFuntion::xglBlitFramebuffer(0, 0, w, h, 0, 0, w, h, XOpenGL::FlagBits::depth_stencil_bit, XOpenGL::FilterType::nearest);

	dest->release(XOpenGL::FrameBufferType::drawBuffer);
	msaaSrc->release(XOpenGL::FrameBufferType::readBuffer);
	XOpenGLFuntion::checkGLError();
}

void XDrawManger::Internal::SlotRenderSizeChanged(XQ::Vec2i size)
{
	if (fboScreen) {
		auto viewport = getRender()->getConvertViewPort();

		fboScreen->updateBufferSize(viewport[2], viewport[3]);
		biltFbo->updateBufferSize(viewport[2], viewport[3]);
		for (auto lay : fboOverlays) {
			lay.second->updateBufferSize(viewport[2], viewport[3]);
		}
	}
}

void XDrawManger::Internal::biltOverlay(int lay)
{
	auto iter =fboOverlays.find(lay);
	if (iter != fboOverlays.end()) {
		bilt(fboOverlays[lay],biltFbo);
	}
}
