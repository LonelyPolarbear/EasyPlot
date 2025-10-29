#include "XOpenGLFramebufferObject.h"
#include "XOpenGLTexture.h"
#include "XOpenGLFuntion.h"
#include <glew/glew.h>

class XOpenGLFramebufferObject::Internal {
public:
	GLuint FBO{0};
	std::shared_ptr< XOpenGLTexture> depthStencilTexture{nullptr};
	std::map<uint32_t, std::shared_ptr< XOpenGLTexture>> colorTextures;		//多个颜色附件
	Attachment depthStencilAttachment{Attachment::Depth};								//深度附件或深度-模板附件
	int width{10};      //纹理宽度
	int height{10};     //纹理高度


	GLint lastFBO =0;
};

XOpenGLFramebufferObject::XOpenGLFramebufferObject():
    d(new Internal)
{
}

XOpenGLFramebufferObject::~XOpenGLFramebufferObject()
{
	release();
	destory();
}

bool XOpenGLFramebufferObject::create()
{
	// 创建FBO
	glGenFramebuffers(1, &d->FBO);
	return d->FBO >0;
}

void XOpenGLFramebufferObject::bind(XOpenGL::FrameBufferType type)
{
	//获取上次绑定的FBO
	GLint tmpLatFbo = 0;
	auto binding =getBindingType(type);
	XOpenGLFuntion::xglGetBindFrameBufferId(binding,tmpLatFbo);

	if (tmpLatFbo == d->FBO)
	{
		return;
	}
	else {
		d->lastFBO = tmpLatFbo;
		XOpenGLFuntion::xglBindFramebuffer(type,d->FBO);
	}
}

void XOpenGLFramebufferObject::release(XOpenGL::FrameBufferType type)
{
	GLint curFbo = 0;
	auto binding = getBindingType(type);
	XOpenGLFuntion::xglGetBindFrameBufferId(binding, curFbo);
	if (curFbo == d->FBO)
	{
		XOpenGLFuntion::xglBindFramebuffer(type, d->lastFBO);
	}
	else {
		//当前FBO未绑定，不做操作
		return;
	}
}

void XOpenGLFramebufferObject::destory()
{
	glDeleteFramebuffers(1, &d->FBO);
	d->FBO = 0;
}

uint32_t XOpenGLFramebufferObject::getId() const
{
	return d->FBO;
}

void XOpenGLFramebufferObject::setWidth(int width)
{
	d->width = width;
}

void XOpenGLFramebufferObject::setHeight(int height)
{
	d->height = height;
}

int XOpenGLFramebufferObject::getWidth() const
{
	return d->width;
}

int XOpenGLFramebufferObject::getHeight() const
{
	return d->height;
}

std::shared_ptr<XOpenGLTexture> XOpenGLFramebufferObject::getColorAttachment(int index) const
{
	return d->colorTextures[index];
}

std::shared_ptr<XOpenGLTexture> XOpenGLFramebufferObject::getDepthAttachment() const
{
	return d->depthStencilTexture;
}

void XOpenGLFramebufferObject::addAttachment(Attachment attachment,
		XOpenGLTexture::TextureFormat internalFormat, 
		XOpenGLTexture::PixelFormat inputdataPixelFormat, 
		XOpenGLTexture::PixelType inputdataPixelType,
		int index)
{
	if (attachment == Attachment::Color) {
		// 创建颜色纹理
		auto colorTexture = makeShareDbObject<XOpenGLTexture>();
		colorTexture->setTarget(XOpenGLTexture::Target::Target2D);
		colorTexture->setInternalFormat(internalFormat);
		colorTexture->create();

		colorTexture->bind();

		colorTexture->setMinificationFilter(XOpenGLTexture::Filter::Nearest);
		colorTexture->setMagnificationFilter(XOpenGLTexture::Filter::Nearest);

		colorTexture->setData(d->width, d->height, 0, /*internalFormat,*/ inputdataPixelFormat, inputdataPixelType, nullptr);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+index, colorTexture->getTarget(), colorTexture->getId(), 0);

		d->colorTextures[index] = colorTexture;
	}
	else  {
		d->depthStencilAttachment = attachment;
		d->depthStencilTexture = makeShareDbObject<XOpenGLTexture>();
		d->depthStencilTexture->setTarget(XOpenGLTexture::Target::Target2D);
		d->depthStencilTexture->setInternalFormat(internalFormat);
		d->depthStencilTexture->create();

		d->depthStencilTexture->bind();

		d->depthStencilTexture->setMinificationFilter(XOpenGLTexture::Filter::Nearest);
		d->depthStencilTexture->setMagnificationFilter(XOpenGLTexture::Filter::Nearest);

		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		//glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		
		d->depthStencilTexture->setData(d->width, d->height, 0/*, internalFormat*/, inputdataPixelFormat, inputdataPixelType, nullptr);

		if (attachment == Attachment::Depth) {
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, d->depthStencilTexture->getTarget(), d->depthStencilTexture->getId(), 0);
		}
		else {
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, d->depthStencilTexture->getTarget(), d->depthStencilTexture->getId(), 0);
		}
	}
}

void XOpenGLFramebufferObject::addAttachmentMSAA(Attachment attachment, 
XOpenGLTexture::TextureFormat internalFormat, 
XOpenGLTexture::PixelFormat inputdataPixelFormat, 
XOpenGLTexture::PixelType inputdataPixelType, 
int index)
{
	if (attachment == Attachment::Color) {
		// 创建颜色纹理
		auto colorTexture = makeShareDbObject<XOpenGLTexture>();
		colorTexture->setTarget(XOpenGLTexture::Target::Target2DMultisample);
		colorTexture->setInternalFormat(internalFormat);
		colorTexture->create();

		colorTexture->bind();
		XOpenGLFuntion::checkGLError();
		//colorTexture->setMinificationFilter(XOpenGLTexture::Filter::Nearest);
		//colorTexture->setMagnificationFilter(XOpenGLTexture::Filter::Nearest);
		XOpenGLFuntion::checkGLError();
		colorTexture->setMultiSample(d->width, d->height, 8, /*internalFormat,*/ inputdataPixelFormat, inputdataPixelType);
		XOpenGLFuntion::checkGLError();
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, colorTexture->getTarget(), colorTexture->getId(), 0);

		d->colorTextures[index] = colorTexture;
	}
	else {
		XOpenGLFuntion::checkGLError();
		d->depthStencilAttachment = attachment;
		d->depthStencilTexture = makeShareDbObject<XOpenGLTexture>();
		d->depthStencilTexture->setTarget(XOpenGLTexture::Target::Target2DMultisample);
		d->depthStencilTexture->setInternalFormat(internalFormat);
		d->depthStencilTexture->create();

		d->depthStencilTexture->bind();

		//d->depthStencilTexture->setMinificationFilter(XOpenGLTexture::Filter::Nearest);
		//d->depthStencilTexture->setMagnificationFilter(XOpenGLTexture::Filter::Nearest);

		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		//glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

		d->depthStencilTexture->setMultiSample(d->width, d->height, 8, /*internalFormat,*/ inputdataPixelFormat, inputdataPixelType);

		if (attachment == Attachment::Depth) {
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, d->depthStencilTexture->getTarget(), d->depthStencilTexture->getId(), 0);
		}
		else {
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, d->depthStencilTexture->getTarget(), d->depthStencilTexture->getId(), 0);
		}
		XOpenGLFuntion::checkGLError();
	}
}


bool XOpenGLFramebufferObject::isComplete() const
{
	auto dddd = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
}

bool XOpenGLFramebufferObject::updateBufferSize(int width, int height)
{
	if(getId()<=0)
		return false;
	//纹理尺寸更改 需要更新视口尺寸
	// 重新设置纹理参数
	d->width = width;
	d->height = height;
	bind();
	

	for(auto& [index, colorTexture]: d->colorTextures){
		auto dataPixelFormat = colorTexture->getInputDataPixelFormat();
		auto dataPixelType = colorTexture->getInputDataPixelType();
		auto internalFormat = colorTexture->getInternalFormat();
		colorTexture->bind();
		colorTexture->setData(d->width, d->height, 0, /*internalFormat,*/ dataPixelFormat, dataPixelType, nullptr);
		
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+index, colorTexture->getTarget(), colorTexture->getId(), 0);
		colorTexture->release();
	}

	if (d->depthStencilTexture) {
		auto dataPixelFormat = d->depthStencilTexture->getInputDataPixelFormat();
		auto dataPixelType = d->depthStencilTexture->getInputDataPixelType();
		auto internalFormat = d->depthStencilTexture->getInternalFormat();
		d->depthStencilTexture->bind();
		d->depthStencilTexture->setData(d->width, d->height, 0, /*internalFormat,*/ dataPixelFormat, dataPixelType, nullptr);
		if (d->depthStencilAttachment == Attachment::Depth) {
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, d->depthStencilTexture->getTarget(), d->depthStencilTexture->getId(), 0);
		}
		else {
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, d->depthStencilTexture->getTarget(), d->depthStencilTexture->getId(), 0);
		}
		
		d->depthStencilTexture->release();
	}

	bool flag = isComplete();

	release();

	return flag;
}

XOpenGL::FrameBufferBindingType XOpenGLFramebufferObject::getBindingType(XOpenGL::FrameBufferType type) const
{
	switch (type)
	{
	case XOpenGL::FrameBufferType::readBuffer:
	return XOpenGL::FrameBufferBindingType::readBufferBinding;
		break;
	case XOpenGL::FrameBufferType::drawBuffer:
		return XOpenGL::FrameBufferBindingType::drawBufferBinding;
		break;
	case XOpenGL::FrameBufferType::framebuffer:
		return XOpenGL::FrameBufferBindingType::framebufferBinding;
		break;
	default:
		break;
	}
}
