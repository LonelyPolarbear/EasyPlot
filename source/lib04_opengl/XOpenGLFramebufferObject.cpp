#include "XOpenGLFramebufferObject.h"
#include "XOpenGLTexture.h"
#include "XOpenGLFuntion.h"
#include <glew/glew.h>

class XOpenGLFramebufferObject::Internal {
public:
	GLuint FBO{0};
	//std::shared_ptr< XOpenGLTexture> depthStencilTexture{nullptr};					//
	std::map<uint32_t, std::shared_ptr< XOpenGLTexture>> colorTextures;		//多个颜色附件
	//Attachment depthStencilAttachment{Attachment::Depth};								//深度附件或深度-模板附件
	int width{10};      //纹理宽度
	int height{10};     //纹理高度

	GLint lastFBO =0;

	std::map<Attachment,sptr<XOpenGLTexture>> depthStencilTextures;		//一般深度模板附件和深度附件或模板附件不可共存，用户需要保证

	sptr<XOpenGLTexture> getDepthAttachment() {
		if (depthStencilTextures.find(Attachment::CombinedDepthStencil) != depthStencilTextures.end()) {
			return depthStencilTextures[CombinedDepthStencil];
		}

		if (depthStencilTextures.find(Attachment::Depth) != depthStencilTextures.end()) {
			return depthStencilTextures[Depth];
		}

		return nullptr;
	}
	sptr<XOpenGLTexture> getSencilAttachment() {
		if (depthStencilTextures.find(Attachment::CombinedDepthStencil) != depthStencilTextures.end()) {
			return depthStencilTextures[CombinedDepthStencil];
		}

		if (depthStencilTextures.find(Attachment::stencil) != depthStencilTextures.end()) {
			return depthStencilTextures[stencil];
		}

		return nullptr;
	}
	sptr<XOpenGLTexture> getDepthStencilAttachment() {
		if (depthStencilTextures.find(Attachment::CombinedDepthStencil) != depthStencilTextures.end()) {
			return depthStencilTextures[CombinedDepthStencil];
		}

		return nullptr;
	}
};

XOpenGLFramebufferObject::XOpenGLFramebufferObject(int width,int height):
    d(new Internal)
{
	d->width = width;
	d->height = height;
}

XOpenGLFramebufferObject::XOpenGLFramebufferObject() :
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
	XOpenGLFuntion::checkGLError();
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
	return d->getDepthAttachment();
}

std::shared_ptr<XOpenGLTexture> XOpenGLFramebufferObject::getStencilAttachment() const
{
	return d->getSencilAttachment();
}

std::shared_ptr<XOpenGLTexture> XOpenGLFramebufferObject::getDepthStencilAttachment() const
{
	return d->getDepthStencilAttachment();
}

void XOpenGLFramebufferObject::addAttachment(Attachment attachment,
		XOpenGLTexture::TextureFormat internalFormat, 
		XOpenGLTexture::PixelFormat inputdataPixelFormat, 
		XOpenGLTexture::PixelType inputdataPixelType,
		int index)
{
	if (attachment == Attachment::Color) {
		auto colorTexture = makeShareDbObject<XOpenGLTexture>();
		colorTexture->setTarget(XOpenGLTexture::Target::Target2D);
		colorTexture->setInternalFormat(internalFormat);
		colorTexture->setExternalFormat(inputdataPixelFormat, inputdataPixelType);
		colorTexture->create();

		colorTexture->bind();

		colorTexture->setMinificationFilter(XOpenGLTexture::Filter::Nearest);
		colorTexture->setMagnificationFilter(XOpenGLTexture::Filter::Nearest);

		colorTexture->texStorage2D(d->width, d->height);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+index, colorTexture->getTarget(), colorTexture->getId(), 0);

		d->colorTextures[index] = colorTexture;
	}
	else  {
		//d->depthStencilAttachment = attachment;
		auto texture = makeShareDbObject<XOpenGLTexture>();
		texture->setTarget(XOpenGLTexture::Target::Target2D);
		texture->setInternalFormat(internalFormat);
		texture->setExternalFormat(inputdataPixelFormat, inputdataPixelType);
		texture->create();

		texture->bind();

		//d->depthStencilTexture->setMinificationFilter(XOpenGLTexture::Filter::Nearest);
		//d->depthStencilTexture->setMagnificationFilter(XOpenGLTexture::Filter::Nearest);

		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		//glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		
		texture->texStorage2D(d->width, d->height);

		d->depthStencilTextures[attachment] = texture;


		if (attachment == Attachment::Depth) {
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture->getTarget(), texture->getId(), 0);
		}
		else if (attachment == Attachment::stencil) {
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, texture->getTarget(), texture->getId(), 0);
		}
		else {
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, texture->getTarget(), texture->getId(), 0);
		}
	}
}

void XOpenGLFramebufferObject::addAttachment(Attachment attachment, sptr<XOpenGLTexture> texture, int index, int level, int layer)
{
	unsigned int Attachment_enum =0;
	switch (attachment)
	{
	case XOpenGLFramebufferObject::CombinedDepthStencil: 
	{
		//d->depthStencilAttachment = attachment;
		//d->depthStencilTexture = texture;
		d->depthStencilTextures[attachment] = texture;
		Attachment_enum = GL_DEPTH_STENCIL_ATTACHMENT;
	}
		break;
	case XOpenGLFramebufferObject::Depth:
	{
		//d->depthStencilAttachment = attachment;
		//d->depthStencilTexture = texture;
		d->depthStencilTextures[attachment] = texture;
		Attachment_enum = GL_DEPTH_ATTACHMENT;
	}
		break;
	case XOpenGLFramebufferObject::Color: {
		Attachment_enum = GL_COLOR_ATTACHMENT0 + index;
		d->colorTextures[index] = texture;
	}
		break;
	case XOpenGLFramebufferObject::stencil:
	{
		//d->depthStencilAttachment = attachment;
		//d->depthStencilTexture = texture;
		d->depthStencilTextures[attachment] = texture;
		Attachment_enum = GL_STENCIL_ATTACHMENT;
	}
		break;
	default:
		break;
	}
	
	if (texture->getTarget() == XOpenGLTexture::Target1D) {
		glFramebufferTexture1D(GL_FRAMEBUFFER, Attachment_enum, texture->getTarget(), texture->getId(), level);
	}
	else if (texture->getTarget() == XOpenGLTexture::Target1DArray) {
		glFramebufferTextureLayer(GL_FRAMEBUFFER, Attachment_enum, texture->getId(), level,layer);
		//glFramebufferTexture2D(GL_FRAMEBUFFER, Attachment_enum, texture->getTarget(), texture->getId(), level);
	}
	else if (texture->getTarget() == XOpenGLTexture::Target2D) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, Attachment_enum, texture->getTarget(), texture->getId(), level);
	}
	else if (texture->getTarget() == XOpenGLTexture::Target2DArray) {
		glFramebufferTextureLayer(GL_FRAMEBUFFER,Attachment_enum, texture->getId(), level, layer);
	}
	else if (texture->getTarget() == XOpenGLTexture::Target3D) {
		//glFramebufferTexture()
		glFramebufferTexture3D(GL_FRAMEBUFFER, Attachment_enum, texture->getTarget(), texture->getId(), level,layer);
	}
	else if (texture->getTarget() == XOpenGLTexture::Target2DMultisample) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, Attachment_enum, texture->getTarget(), texture->getId(), level);
	}
}

void XOpenGLFramebufferObject::addAttachmentMSAA(Attachment attachment, 
XOpenGLTexture::TextureFormat internalFormat, 
XOpenGLTexture::PixelFormat inputdataPixelFormat, 
XOpenGLTexture::PixelType inputdataPixelType, 
int sampleNum,
int index)
{
	if (attachment == Attachment::Color) {
		// 创建颜色纹理
		auto colorTexture = makeShareDbObject<XOpenGLTexture>();
		colorTexture->setTarget(XOpenGLTexture::Target::Target2DMultisample);
		colorTexture->setInternalFormat(internalFormat);
		colorTexture->setExternalFormat(inputdataPixelFormat,inputdataPixelType);
		colorTexture->create();

		colorTexture->bind();
		XOpenGLFuntion::checkGLError();
		XOpenGLFuntion::checkGLError(); 
		colorTexture->texStorage2DMultiSample(d->width, d->height, sampleNum);
		XOpenGLFuntion::checkGLError();
		//glFramebufferTexture2DMultisample
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, colorTexture->getTarget(), colorTexture->getId(),0);

		d->colorTextures[index] = colorTexture;
	}
	else {
		XOpenGLFuntion::checkGLError();
		//d->depthStencilAttachment = attachment;
		auto texture = makeShareDbObject<XOpenGLTexture>();
		texture->setTarget(XOpenGLTexture::Target::Target2DMultisample);
		texture->setInternalFormat(internalFormat);
		texture->setExternalFormat(inputdataPixelFormat, inputdataPixelType);
		texture->create();

		texture->bind();

		//d->depthStencilTexture->setMinificationFilter(XOpenGLTexture::Filter::Nearest);
		//d->depthStencilTexture->setMagnificationFilter(XOpenGLTexture::Filter::Nearest);

		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		//glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

		texture->texStorage2DMultiSample(d->width, d->height, sampleNum);

		d->depthStencilTextures[attachment] = texture;

		if (attachment == Attachment::Depth) {
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture->getTarget(), texture->getId(), 0);
		}
		else if(attachment == Attachment::stencil) {
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, texture ->getTarget(), texture->getId(), 0);
		}
		else {
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, texture->getTarget(), texture->getId(), 0);
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
	if(d->width == width && d->height == height)
		return false;
	d->width = width;
	d->height = height;
	bind();
	int sampleNum = 8;

	for(auto& [index, colorTexture]: d->colorTextures){
		colorTexture->bind();
		//colorTexture->texStorage2D(d->width, d->height);
		if (colorTexture->getTarget() == XOpenGLTexture::Target::Target2DMultisample) {
			colorTexture->texStorage2DMultiSample(d->width, d->height, sampleNum);
		}
		else {
			colorTexture->texStorage2D(d->width, d->height);
		}

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, colorTexture->getTarget(), colorTexture->getId(), 0);
		
		colorTexture->release();
	}

	for (auto s : d->depthStencilTextures) {
		auto attachment = s.first;
		auto texture = s.second;

		texture->bind();

		if (texture->getTarget() == XOpenGLTexture::Target::Target2DMultisample) {
			texture->texStorage2DMultiSample(d->width, d->height, sampleNum);
		}
		else {
			texture->texStorage2D(d->width, d->height);
		}

		if (attachment == Attachment::Depth) {
			
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture->getTarget(), texture->getId(), 0);
		}
		else if(attachment == Attachment::stencil) {

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, texture->getTarget(), texture->getId(), 0);
		}
		else {
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, texture->getTarget(), texture->getId(), 0);
		}

		texture->release();
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

void XOpenGLFramebufferObject::readPixel(Attachment attachment, int startx, int starty,int width,int height, XOpenGL::TextureExternalFormat externalFormat, XOpenGL::DataType externalPixelType, void* data, int index /*=0 */)
{
	bind(XOpenGL::FrameBufferType::readBuffer);
	//fbo->bind(XOpenGL::FrameBufferType::framebuffer);
	if (attachment == Attachment::Color) {
		glReadBuffer(GL_COLOR_ATTACHMENT0+index);
	}
	else {
		glReadBuffer(GL_NONE);
	}
	

	XQ::Vec4u object_data;
	XOpenGLFuntion::xglReadPixels(startx, starty, width, height, externalFormat, externalPixelType, data);

	release(XOpenGL::FrameBufferType::readBuffer);
}
