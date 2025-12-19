#include <glew/glew.h>
#include "XOpenGLTexture.h"
#include "XOpenGLBuffer.h"
#include "XOpenGLFramebufferObject.h"
#include "XOpenGLFuntion.h"
#include <iostream>
class XOpenGLTexture::Internal {
public:
	GLuint textureId{0};
	Target target{Target::Target2D};
	int width{0};
	int height{0};
	int layer{1};
	int samples{0};
	XOpenGLTexture::TextureFormat internalFormat;
	XOpenGLTexture::PixelFormat dataFormat;				//对于MSAA，该值不起作用
	XOpenGLTexture::PixelType datatype;						//对于MSAA，该值不起作用

	int lastUnitBindTexture{0};										//当前指定的纹理单元绑定的纹理ID
};

XOpenGLTexture::XOpenGLTexture():d(new Internal)
{
}

XOpenGLTexture::~XOpenGLTexture()
{
	destroy();
}

void XOpenGLTexture::setTarget(Target target)
{
	d->target = target;
}

void XOpenGLTexture::setInternalFormat(TextureFormat format)
{
	d->internalFormat = format;
}

XOpenGLTexture::Target XOpenGLTexture::getTarget() const
{
	return d->target;
}

uint32_t XOpenGLTexture::getId() const
{
	return d->textureId;
}

bool XOpenGLTexture::create()
{
	if (d->textureId != 0)
		return true;
	glGenTextures(1, &d->textureId);
	XOpenGLFuntion::checkGLError();
	return d->textureId != 0;
}

void XOpenGLTexture::destroy()
{
	if (d->textureId != 0) {
		glDeleteTextures(1, &d->textureId);
		d->textureId = 0;	
	}
}

void XOpenGLTexture::bind()
{
	glBindTexture(d->target, d->textureId);
	XOpenGLFuntion::checkGLError();
}

void XOpenGLTexture::bindUnit(unsigned int unit)
{
	auto bindType = getTextureBindType();

	//获取当前纹理单元绑定的纹理对象
	auto curBindTextureId = XOpenGLFuntion::xglGetTextureUnitBindTexture(unit,bindType);
	if(curBindTextureId == d->textureId)
		return;

	d->lastUnitBindTexture = curBindTextureId;

	//glBindTextureUnit(unit, d->textureId);  //glBindTextureUnit 是 OpenGL 4.5 版本 中引入的核心函数

	//先获取之前激活的纹理单元
	auto prevActiveUnit = XOpenGLFuntion::xglGetActiveTexture();

	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(d->target, d->textureId);

	//恢复之前绑定的纹理对象
	glActiveTexture(GL_TEXTURE0+ prevActiveUnit);
}

void XOpenGLTexture::release()
{
	glBindTexture(d->target, 0);
	XOpenGLFuntion::checkGLError();
}

void XOpenGLTexture::releaseUnit(unsigned int unit)
{
	auto bindType = getTextureBindType();
	//获取当前纹理单元绑定的纹理对象
	auto curBindTextureId = XOpenGLFuntion::xglGetTextureUnitBindTexture(unit, bindType);

	if(curBindTextureId != d->textureId)
		return;

	auto prevActiveUnit = XOpenGLFuntion::xglGetActiveTexture();

	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(d->target, d->lastUnitBindTexture);

	//恢复之前绑定的纹理对象
	glActiveTexture(GL_TEXTURE0 + prevActiveUnit);
}

void XOpenGLTexture::bindBuffer(std::shared_ptr<XOpenGLBuffer> buffer, TextureFormat format)
{
	glTexBuffer(GL_TEXTURE_BUFFER, format, buffer->bufferId());	    //将颜色数据存储到纹理缓存对象
}

void XOpenGLTexture::setMagnificationFilter(XOpenGLTexture::Filter filter)
{
	glTexParameteri(d->target, GL_TEXTURE_MIN_FILTER, filter);  
}

void XOpenGLTexture::setMinificationFilter(XOpenGLTexture::Filter filter)
{
	glTexParameteri(d->target, GL_TEXTURE_MAG_FILTER, filter);
}

void XOpenGLTexture::setWrapMode(XOpenGLTexture::CoordinateDirection direction, XOpenGLTexture::WrapMode mode)
{
	glTexParameteri(d->target, direction, mode); 
}

void XOpenGLTexture::setData(
													int width, 
													int height, 
													int level, 
													XOpenGLTexture::PixelFormat dataFormat, 
													XOpenGLTexture::PixelType datatype,
													const void* data)
{
	d->width = width;
	d->height = height;
	d->dataFormat = dataFormat;
	d->datatype = datatype;
	glTexImage2D(d->target, level/*mimmap层级*/, d->internalFormat, width, height, 0/*历史遗留问题*/,dataFormat , datatype, data);
	XOpenGLFuntion::checkGLError();
}

void XOpenGLTexture::setMultiSample(int width, int height, int sampleNum, XOpenGLTexture::PixelFormat dataFormat, XOpenGLTexture::PixelType type)
{

	d->width = width;
	d->height = height;
	d->dataFormat = dataFormat;
	d->datatype = type;
	d->samples = sampleNum;

	glTexImage2DMultisample(d->target, sampleNum, d->internalFormat, width, height, GL_TRUE);
}

void XOpenGLTexture::setData(
										int width, 
										int height, 
										int level, 
	
										XOpenGLTexture::PixelFormat dataFormat, 
										XOpenGLTexture::PixelType type, 
										std::vector< const void*> datas)

{
	//用于设置数组纹理

	d->width = width;
	d->height = height;
	d->dataFormat = dataFormat;
	d->datatype = type;
	d->layer = datas.size();

	glTexImage3D(d->target, level, d->internalFormat, width, height, datas.size(), 0, dataFormat, type, nullptr);
	XOpenGLFuntion::checkGLError();
	//glTextureSubImage2D
	for (int lay = 0; lay < datas.size(); ++lay) {
		glTexSubImage3D(d->target,
			level,
			0, 0, lay,
			width, height, 1,
			dataFormat,
			type,
			datas[lay]);
		XOpenGLFuntion::checkGLError();
	}
	;
}

void XOpenGLTexture::setData(
										CubeMapFace cubeFace, 
										int width, 
										int height, 
										int level, 
										XOpenGLTexture::PixelFormat dataFormat, 
										XOpenGLTexture::PixelType datatype, 
										const void* data)
{
	glTexImage2D(cubeFace, level/*mimmap层级*/, d->internalFormat, width, height, 0/*历史遗留问题*/, dataFormat, datatype, data);
}

void XOpenGLTexture::GenerateMipmap()
{
	glGenerateMipmap(d->target); //让系统自动处理多级渐远纹理
}

XOpenGLTexture::TextureFormat XOpenGLTexture::getInternalFormat() const
{
	return d->internalFormat;
}

XOpenGLTexture::PixelFormat XOpenGLTexture::getInputDataPixelFormat() const
{
	return d->dataFormat;
}

XOpenGLTexture::PixelType XOpenGLTexture::getInputDataPixelType() const
{
	return d->datatype;
}

sptr<XOpenGLBuffer> XOpenGLTexture::map(int alignment)
{
	//设值对齐
	//获取每行的实际宽度
	auto originRowlen = d->width* getInternalFormatSize(d->internalFormat);
	auto aligmentLen = alignment ==0 ? originRowlen : ((originRowlen + alignment - 1) / alignment)* alignment;

	auto oldPackAlignment = XOpenGLFuntion::xglPixelStorei(XOpenGL::PixelStoreParameter::pack_alignment, alignment);

	auto pbo = makeShareDbObject<XOpenGLBuffer>();
	pbo->setBufferType(XOpenGLBuffer::PixelPackBuffer);
	pbo->setUsagePattern(XOpenGLBuffer::UsagePattern::StreamRead);
	pbo->create();
	pbo->bind();

	auto expactedSize =  d->height *d->layer* aligmentLen;
	auto pboSize = pbo->bufferSize();

	if (pboSize != expactedSize) {
		pbo->allocate(expactedSize);
	}

	bind();
	if (getTarget() == Target2DArray) {
		//是在 OpenGL 4.5 中引入的
		glGetTextureSubImage(getId(),0,
		0,0,0,
		d->width,d->height,d->layer,
		d->dataFormat,
		d->datatype,
		expactedSize,
		0);
	}
	else {
		glGetTexImage(
			d->target,							// 纹理目标
			0,											// mipmap级别
			d->dataFormat,					// 格式（必须匹配纹理的内部格式）
			d->datatype,						// 类型（必须匹配纹理的数据类型）
			0											// 偏移量（使用PBO时设为0）
		);
	}

	
	XOpenGLFuntion::checkGLError();

	release();
	oldPackAlignment = XOpenGLFuntion::xglPixelStorei(XOpenGL::PixelStoreParameter::pack_alignment, oldPackAlignment);
	return pbo;
}

sptr<XOpenGLBuffer> XOpenGLTexture::map(int pboWidth, int pboHeight, int x, int y)
{
	auto alignment =1;
	auto oldPackRowLength = XOpenGLFuntion::xglPixelStorei(XOpenGL::PixelStoreParameter::pack_row_length, pboWidth);
	auto oldPackAlignment = XOpenGLFuntion::xglPixelStorei(XOpenGL::PixelStoreParameter::pack_alignment, alignment);
	auto oldPackSkipRows = XOpenGLFuntion::xglPixelStorei(XOpenGL::PixelStoreParameter::pack_skip_rows, y);
	auto oldPackSkipPixel = XOpenGLFuntion::xglPixelStorei(XOpenGL::PixelStoreParameter::pack_skip_pixels, x);

	auto originRowlen = d->width * getInternalFormatSize(d->internalFormat);
	auto aligmentLen = alignment == 0 ? originRowlen : ((originRowlen + alignment - 1) / alignment) * alignment;

	auto pbo = makeShareDbObject<XOpenGLBuffer>();
	pbo->setBufferType(XOpenGLBuffer::PixelPackBuffer);
	pbo->setUsagePattern(XOpenGLBuffer::UsagePattern::StreamRead);
	pbo->create();
	pbo->bind();
	
	auto pboSize = pbo->bufferSize();

	if (pboSize != pboHeight * aligmentLen) {
		pbo->allocate(pboHeight * aligmentLen); // RGBA8格式
	}

	bind();
	if (getTarget() == Target2DArray) {
		auto expactedSize = d->height * d->layer * aligmentLen;
		//是在 OpenGL 4.5 中引入的
		glGetTextureSubImage(getId(), 0,
			0, 0, 0,
			d->width, d->height, d->layer,
			d->dataFormat,
			d->datatype,
			expactedSize,
			0);
	}else{
		glGetTexImage(
			d->target,							// 纹理目标
			0,											// mipmap级别
			d->dataFormat,					// 格式（必须匹配纹理的内部格式）
			d->datatype,						// 类型（必须匹配纹理的数据类型）
			0											// 偏移量（使用PBO时设为0）
		);
	}

	release();

	 oldPackRowLength = XOpenGLFuntion::xglPixelStorei(XOpenGL::PixelStoreParameter::pack_row_length, oldPackRowLength);
	 oldPackAlignment = XOpenGLFuntion::xglPixelStorei(XOpenGL::PixelStoreParameter::pack_alignment, oldPackAlignment);
	 oldPackSkipRows = XOpenGLFuntion::xglPixelStorei(XOpenGL::PixelStoreParameter::pack_skip_rows, oldPackSkipRows);
	 oldPackSkipPixel = XOpenGLFuntion::xglPixelStorei(XOpenGL::PixelStoreParameter::pack_skip_pixels, oldPackSkipPixel);
	return pbo;
}

sptr<XOpenGLBuffer> XOpenGLTexture::mapMultiSampleColor(unsigned int fboId)
{
	auto alignment =1;
	auto originRowlen = d->width * getInternalFormatSize(d->internalFormat);
	auto aligmentLen = alignment == 0 ? originRowlen : ((originRowlen + alignment - 1) / alignment) * alignment;

	auto pbo = makeShareDbObject<XOpenGLBuffer>();
	pbo->setBufferType(XOpenGLBuffer::PixelPackBuffer);
	pbo->setUsagePattern(XOpenGLBuffer::UsagePattern::StreamRead);
	pbo->create();
	pbo->bind();
	//pbo->allocate(d->height * aligmentLen); // RGBA8格式

	auto resoveFbo = makeShareDbObject<XOpenGLFramebufferObject>();
	resoveFbo->create();
	resoveFbo->setHeight(d->height);
	resoveFbo->setWidth(d->width);
	resoveFbo->bind();
	resoveFbo->addAttachment(XOpenGLFramebufferObject::Attachment::Color, d->internalFormat, d->dataFormat, d->datatype, 0);
	bool flag = resoveFbo->isComplete();
	resoveFbo->release();

	XOpenGLFuntion::xglBindFramebuffer(XOpenGL::FrameBufferType::readBuffer, fboId);
	XOpenGLFuntion::xglBindFramebuffer(XOpenGL::FrameBufferType::drawBuffer, resoveFbo->getId());

	XOpenGLFuntion::xglBlitFramebuffer(0, 0, d->width, d->height, 0, 0, d->width, d->height,
		XOpenGL::FlagBits::color_buffer_bit, XOpenGL::FilterType::nearest);

	auto texture = resoveFbo->getColorAttachment();

	pbo->bind();
	auto pboSize = pbo->bufferSize();

	if (pboSize !=  d->height * aligmentLen) {
		pbo->allocate(d->height * aligmentLen); // RGBA8格式
	}

	texture->bind();
	glGetTexImage(
		texture->getTarget(),			// 纹理目标
		0,											// mipmap级别
		d->dataFormat,					// 格式（必须匹配纹理的内部格式）
		d->datatype,						// 类型（必须匹配纹理的数据类型）
		0											// 偏移量（使用PBO时设为0）
	);
	return pbo;
}

sptr<XOpenGLBuffer> XOpenGLTexture::mapMultiSampleDepth(unsigned int fboId)
{
	auto alignment = 1;
	auto originRowlen = d->width * getInternalFormatSize(d->internalFormat);
	auto aligmentLen = alignment == 0 ? originRowlen : ((originRowlen + alignment - 1) / alignment) * alignment;

	auto pbo = makeShareDbObject<XOpenGLBuffer>();
	pbo->setBufferType(XOpenGLBuffer::PixelPackBuffer);
	pbo->setUsagePattern(XOpenGLBuffer::UsagePattern::StreamRead);
	pbo->create();
	pbo->bind();

	//pbo->allocate(d->height * aligmentLen); // RGBA8格式

	auto resoveFbo = makeShareDbObject<XOpenGLFramebufferObject>();
	resoveFbo->create();
	resoveFbo->setHeight(d->height);
	resoveFbo->setWidth(d->width);
	resoveFbo->bind();
	resoveFbo->addAttachment(XOpenGLFramebufferObject::Attachment::Depth, d->internalFormat, d->dataFormat, d->datatype, 0);
	bool flag = resoveFbo->isComplete();
	resoveFbo->release();

	XOpenGLFuntion::checkGLError();

	XOpenGLFuntion::xglBindFramebuffer(XOpenGL::FrameBufferType::readBuffer, fboId);
	XOpenGLFuntion::xglBindFramebuffer(XOpenGL::FrameBufferType::drawBuffer, resoveFbo->getId());

	XOpenGLFuntion::checkGLError();

	XOpenGLFuntion::xglBlitFramebuffer(0, 0, d->width, d->height, 0, 0, d->width, d->height,
		XOpenGL::FlagBits::depth_buffer_bit, XOpenGL::FilterType::nearest);
	auto texture = resoveFbo->getDepthAttachment();

	//d->pbo->bind();
	auto pboSize = pbo->bufferSize();

	if (pboSize != d->height * aligmentLen) {
		pbo->allocate(d->height * aligmentLen); // RGBA8格式
	}

	texture->bind();
	glGetTexImage(
		texture->getTarget(),			// 纹理目标
		0,											// mipmap级别
		d->dataFormat,					// 格式（必须匹配纹理的内部格式）
		d->datatype,						// 类型（必须匹配纹理的数据类型）
		0											// 偏移量（使用PBO时设为0）
	);

	return pbo;
}

unsigned int XOpenGLTexture::getInternalFormatSize(XOpenGLTexture::TextureFormat format)
{
	switch (format)
	{
	case XOpenGLTexture::NoFormat:
		return 0;
	case XOpenGLTexture::R8_UNorm:
		return 1;
	case XOpenGLTexture::RG8_UNorm:
		return 2;
	case XOpenGLTexture::RGB8_UNorm:
		return 3;
	case XOpenGLTexture::RGBA8_UNorm:
		return 4;
	case XOpenGLTexture::R16_UNorm:
		return 2;
	case XOpenGLTexture::RG16_UNorm:
		return 4;
	case XOpenGLTexture::RGB16_UNorm:
		return 6;
	case XOpenGLTexture::RGBA16_UNorm:
		return 8;
	case XOpenGLTexture::R8_SNorm:
		return 1;
	case XOpenGLTexture::RG8_SNorm:
		return 2;
	case XOpenGLTexture::RGB8_SNorm:
		return 3;
	case XOpenGLTexture::RGBA8_SNorm:
		return 4;
	case XOpenGLTexture::R16_SNorm:
		return 2;
	case XOpenGLTexture::RG16_SNorm:
		return 4;
	case XOpenGLTexture::RGB16_SNorm:
		return 6;
	case XOpenGLTexture::RGBA16_SNorm:
		return 8;
	case XOpenGLTexture::R8U:
		return 1;
	case XOpenGLTexture::RG8U:
		return 2;
	case XOpenGLTexture::RGB8U:
		return 3;
	case XOpenGLTexture::RGBA8U:
		return 4;
	case XOpenGLTexture::R16U:
		return 2;
	case XOpenGLTexture::RG16U:
		return 4;
	case XOpenGLTexture::RGB16U:
		return 6;
	case XOpenGLTexture::RGBA16U:
		return 8;
	case XOpenGLTexture::R32U:
		return 4;
	case XOpenGLTexture::RG32U:
		return 8;
	case XOpenGLTexture::RGB32U:
		return 12;
	case XOpenGLTexture::RGBA32U:
		return 16;
	case XOpenGLTexture::R8I:
		return 1;
	case XOpenGLTexture::RG8I:
		return 2;
	case XOpenGLTexture::RGB8I:
		return 3;
	case XOpenGLTexture::RGBA8I:
		return 4;
	case XOpenGLTexture::R16I:
		return 2;
	case XOpenGLTexture::RG16I:
		return 4;
	case XOpenGLTexture::RGB16I:
		return 6;
	case XOpenGLTexture::RGBA16I:
		return 8;
	case XOpenGLTexture::R32I:
		return 4;
	case XOpenGLTexture::RG32I:
		return 8;
	case XOpenGLTexture::RGB32I:
		return 12;
	case XOpenGLTexture::RGBA32I:
		return 16;
	case XOpenGLTexture::R16F:
		return 2;
	case XOpenGLTexture::RG16F:
		return 4;
	case XOpenGLTexture::RGB16F:
		return 6;
	case XOpenGLTexture::RGBA16F:
		return 8;
	case XOpenGLTexture::R32F:
		return 4;
	case XOpenGLTexture::RG32F:
		return 8;
	case XOpenGLTexture::RGB32F:
		return 12;
	case XOpenGLTexture::RGBA32F:
		return 16;
	case XOpenGLTexture::RGB9E5:
		return 3;
	case XOpenGLTexture::RG11B10F:
		return 4;
	case XOpenGLTexture::RG3B2:
		return 3;
	case XOpenGLTexture::R5G6B5:
		return 2;
	case XOpenGLTexture::RGB5A1:
		return 2;
	case XOpenGLTexture::RGBA4:
		return 2;
	case XOpenGLTexture::RGB10A2:
		return 4;
	case XOpenGLTexture::D16:
		return 2;
	case XOpenGLTexture::D24:
		return 4;
	case XOpenGLTexture::D24S8:
		return 6;
	case XOpenGLTexture::D32:
		return 4;
	case XOpenGLTexture::D32F:
		return 4;
	case XOpenGLTexture::D32FS8X24:
		return 8;
	case XOpenGLTexture::S8:
		return 1;
	case XOpenGLTexture::RGB_DXT1:
		return 0;
	case XOpenGLTexture::RGBA_DXT1:
		return 0;
	case XOpenGLTexture::RGBA_DXT3:
		return 0;
	case XOpenGLTexture::RGBA_DXT5:
		return 0;
	case XOpenGLTexture::R_ATI1N_UNorm:
		return 0;
	case XOpenGLTexture::R_ATI1N_SNorm:
		return 0;
	case XOpenGLTexture::RG_ATI2N_UNorm:
		return 0;
	case XOpenGLTexture::RG_ATI2N_SNorm:
		return 0;
	case XOpenGLTexture::RGB_BP_UNSIGNED_FLOAT:
		return 0;
	case XOpenGLTexture::RGB_BP_SIGNED_FLOAT:
		return 0;
	case XOpenGLTexture::RGB_BP_UNorm:
		break;
	case XOpenGLTexture::R11_EAC_UNorm:
		break;
	case XOpenGLTexture::R11_EAC_SNorm:
		break;
	case XOpenGLTexture::RG11_EAC_UNorm:
		break;
	case XOpenGLTexture::RG11_EAC_SNorm:
		break;
	case XOpenGLTexture::RGB8_ETC2:
		break;
	case XOpenGLTexture::SRGB8_ETC2:
		break;
	case XOpenGLTexture::RGB8_PunchThrough_Alpha1_ETC2:
		break;
	case XOpenGLTexture::SRGB8_PunchThrough_Alpha1_ETC2:
		break;
	case XOpenGLTexture::RGBA8_ETC2_EAC:
		break;
	case XOpenGLTexture::SRGB8_Alpha8_ETC2_EAC:
		break;
	case XOpenGLTexture::RGB8_ETC1:
		break;
	case XOpenGLTexture::RGBA_ASTC_4x4:
		break;
	case XOpenGLTexture::RGBA_ASTC_5x4:
		break;
	case XOpenGLTexture::RGBA_ASTC_5x5:
		break;
	case XOpenGLTexture::RGBA_ASTC_6x5:
		break;
	case XOpenGLTexture::RGBA_ASTC_6x6:
		break;
	case XOpenGLTexture::RGBA_ASTC_8x5:
		break;
	case XOpenGLTexture::RGBA_ASTC_8x6:
		break;
	case XOpenGLTexture::RGBA_ASTC_8x8:
		break;
	case XOpenGLTexture::RGBA_ASTC_10x5:
		break;
	case XOpenGLTexture::RGBA_ASTC_10x6:
		break;
	case XOpenGLTexture::RGBA_ASTC_10x8:
		break;
	case XOpenGLTexture::RGBA_ASTC_10x10:
		break;
	case XOpenGLTexture::RGBA_ASTC_12x10:
		break;
	case XOpenGLTexture::RGBA_ASTC_12x12:
		break;
	case XOpenGLTexture::SRGB8_Alpha8_ASTC_4x4:
		break;
	case XOpenGLTexture::SRGB8_Alpha8_ASTC_5x4:
		break;
	case XOpenGLTexture::SRGB8_Alpha8_ASTC_5x5:
		break;
	case XOpenGLTexture::SRGB8_Alpha8_ASTC_6x5:
		break;
	case XOpenGLTexture::SRGB8_Alpha8_ASTC_6x6:
		break;
	case XOpenGLTexture::SRGB8_Alpha8_ASTC_8x5:
		break;
	case XOpenGLTexture::SRGB8_Alpha8_ASTC_8x6:
		break;
	case XOpenGLTexture::SRGB8_Alpha8_ASTC_8x8:
		break;
	case XOpenGLTexture::SRGB8_Alpha8_ASTC_10x5:
		break;
	case XOpenGLTexture::SRGB8_Alpha8_ASTC_10x6:
		break;
	case XOpenGLTexture::SRGB8_Alpha8_ASTC_10x8:
		break;
	case XOpenGLTexture::SRGB8_Alpha8_ASTC_10x10:
		break;
	case XOpenGLTexture::SRGB8_Alpha8_ASTC_12x10:
		break;
	case XOpenGLTexture::SRGB8_Alpha8_ASTC_12x12:
		break;
	case XOpenGLTexture::SRGB8:
		break;
	case XOpenGLTexture::SRGB8_Alpha8:
		break;
	case XOpenGLTexture::SRGB_DXT1:
		break;
	case XOpenGLTexture::SRGB_Alpha_DXT1:
		break;
	case XOpenGLTexture::SRGB_Alpha_DXT3:
		break;
	case XOpenGLTexture::SRGB_Alpha_DXT5:
		break;
	case XOpenGLTexture::SRGB_BP_UNorm:
		break;
	default:
		break;
	}
	return 0;
}

XOpenGL::TextureBindingType XOpenGLTexture::getTextureBindType() const
{
	switch (d->target)
	{
	default:
		break;
	case XOpenGLTexture::Target::Target1D:
		return XOpenGL::TextureBindingType::XGL_TEXTURE_BINDING_1D;
	case XOpenGLTexture::Target::Target1DArray:
		return XOpenGL::TextureBindingType::XGL_TEXTURE_BINDING_1D_ARRAY;
	case XOpenGLTexture::Target::Target2D:
		return XOpenGL::TextureBindingType::XGL_TEXTURE_BINDING_2D;
	case XOpenGLTexture::Target::Target2DArray:
		return XOpenGL::TextureBindingType::XGL_TEXTURE_BINDING_2D_ARRAY;
	case XOpenGLTexture::Target::Target3D:
		return XOpenGL::TextureBindingType::XGL_TEXTURE_BINDING_3D;
	case XOpenGLTexture::Target::TargetCubeMap:
		return XOpenGL::TextureBindingType::XGL_TEXTURE_BINDING_CUBE_MAP;
	case XOpenGLTexture::Target::TargetCubeMapArray:
		return XOpenGL::TextureBindingType::XGL_TEXTURE_BINDING_CUBE_MAP_ARRAY;
	case XOpenGLTexture::Target::TargetRectangle:
		return XOpenGL::TextureBindingType::XGL_TEXTURE_BINDING_RECTANGLE;
	case XOpenGLTexture::Target::TargetBuffer:
		return XOpenGL::TextureBindingType::XGL_TEXTURE_BINDING_BUFFER;
	case XOpenGLTexture::Target::Target2DMultisample:
		return XOpenGL::TextureBindingType::XGL_TEXTURE_BINDING_2D_MULTISAMPLE;
	case XOpenGLTexture::Target::Target2DMultisampleArray:
		return XOpenGL::TextureBindingType::XGL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY;
	}
}