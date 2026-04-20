#include <glew/glew.h>
#include "XOpenGLTexture.h"
#include "XOpenGLBuffer.h"
#include "XOpenGLFramebufferObject.h"
#include "XOpenGLFuntion.h"
#include <nlohmann/json.hpp>
class XOpenGLTexture::Internal {
public:
	GLuint textureId{0};
	Target target{Target::Target2D};
	int width{1};
	int height{1};
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

bool XOpenGLTexture::create()
{
	if (d->textureId != 0)
		return true;
	glGenTextures(1, &d->textureId);
	XOpenGLFuntion::checkGLError();
	return d->textureId != 0;
}

void XOpenGLTexture::setInternalFormat(TextureFormat format)
{
	d->internalFormat = format;
}

void XOpenGLTexture::setExternalFormat(PixelFormat inputDataFormat, PixelType inputDataPixelType)
{
	d->dataFormat = inputDataFormat;
	d->datatype = inputDataPixelType;
}

void XOpenGLTexture::texStorage1D(int width)
{
	auto allocatedSize = getStorgeSize();
	
	if (allocatedSize == 0) {
		//尚未分配空间
		d->width = width;
		//glTexImage1D(d->target, 0, d->internalFormat, width, 0, d->dataFormat, d->datatype, nullptr);
		glTexStorage1D(d->target, 1, d->internalFormat, width);		//4.2版本中的扩展
		//glTextureStorage1D()
		XOpenGLFuntion::checkGLError();
		return;
	}
	
	if (computeStorgeSize(width, 1, 1, d->internalFormat) == allocatedSize && d->width == width) {
		//尺寸相同，不需要重新分配
		return;
	}

	//尺寸不同，需要先重新创建，再次分配空间
	//后续可能需要发射信号
	destroy();

	create();
	bind();
	texStorage1D(width);
}

void XOpenGLTexture::texStorage2D(int width, int height)
{
	auto allocatedSize = getStorgeSize();

	if (allocatedSize == 0) {
		//尚未分配空间
		d->width = width;
		d->height = height;
		//glTexImage2D(d->target, 0/*mimmap层级*/, d->internalFormat, width, height, 0/*历史遗留问题*/, d->dataFormat, d->datatype, nullptr);
		glTexStorage2D(d->target, 1, d->internalFormat, width, height);		//4.2版本中的扩展
		XOpenGLFuntion::checkGLError();
		return;
	}

	if (computeStorgeSize(width, height, 1, d->internalFormat) == allocatedSize && d->width == width &&d->height == height) {
		//尺寸相同，不需要重新分配
		return;
	}

	destroy();

	create();
	bind();
	texStorage2D(width,height);
}

void XOpenGLTexture::texStorage2DMultiSample(int width, int height, int sampleNum)
{
	auto sm = *XOpenGLFuntion::xGetTextureSampleNum(d->textureId, (XOpenGL::TextureTarget)d->target);
	auto allocatedSize = getStorgeSize()*sm;

	if (allocatedSize == 0) {
		//尚未分配空间
		d->width = width;
		d->height = height;
		d->samples = sampleNum;
		glTexImage2DMultisample(d->target, d->samples, d->internalFormat, width, height, GL_TRUE);
		return;
	}

	if (sampleNum*computeStorgeSize(width, height, 1, d->internalFormat) == allocatedSize && d->width == width && d->height == height) {
		//尺寸相同，不需要重新分配
		return;
	}

	destroy();

	create();
	bind();
	texStorage2DMultiSample(width, height,sampleNum);
}

void XOpenGLTexture::texStorage3D(int width, int height, int depth)
{
	auto allocatedSize = getStorgeSize();

	if (allocatedSize == 0) {
		//尚未分配空间
		d->width = width;
		d->height = height;
		d->layer = depth;
		glTexStorage3D(d->target, 1, d->internalFormat, width, height, depth);
		//glTexImage3D(d->target, 0, d->internalFormat, width, height, depth, 0, d->dataFormat, d->datatype, nullptr);
		return;
	}

	if (computeStorgeSize(width, height, d->layer, d->internalFormat) == allocatedSize && 
																									d->width == width && 
																									d->height == height &&
																									d->layer == depth) {
		//尺寸相同，不需要重新分配
		return;
	}

	destroy();

	create();
	bind();
	texStorage3D(width, height,depth);
}

void XOpenGLTexture::texStorage3DMultiSample(int width, int height, int depth, int sampleNum)
{
	auto sm = *XOpenGLFuntion::xGetTextureSampleNum(d->textureId, (XOpenGL::TextureTarget)d->target);
	auto allocatedSize = getStorgeSize() * sm;

	if (allocatedSize == 0) {
		//尚未分配空间
		d->width = width;
		d->height = height;
		d->layer = depth;
		d->samples = sampleNum;
		//glTexImage3DMultisample(d->target, d->samples, d->internalFormat, width, height, depth, GL_TRUE);
		glTexStorage3D(d->target, 1, d->internalFormat, width, height, depth);		//4.2版本中的扩展
		return;
	}

	if (sampleNum * computeStorgeSize(width, height, height, d->internalFormat) == allocatedSize && 
	d->width == width && 
	d->height == height &&
	d->layer == depth
	) {
		//尺寸相同，不需要重新分配
		return;
	}

	destroy();

	create();
	bind();
	texStorage3DMultiSample(width, height, depth, sampleNum);
}

void XOpenGLTexture::setSubData1D(int xoffset, int width, const void* data)
{
	auto alignment = 1;
	auto oldPackAlignment = XOpenGLFuntion::xglPixelStorei(XOpenGL::PixelStoreParameter::unpack_alignment, alignment);
	glTexSubImage1D(d->target,0,xoffset,width,d->dataFormat,d->datatype,data);
	glGenerateMipmap(d->target);
	XOpenGLFuntion::xglPixelStorei(XOpenGL::PixelStoreParameter::unpack_alignment, oldPackAlignment);
	XOpenGLFuntion::checkGLError();
}

void XOpenGLTexture::setSubData2D(int xoffset, int yoffset, int width, int height, const void* data)
{
	auto alignment = 1;
	auto oldPackAlignment = XOpenGLFuntion::xglPixelStorei(XOpenGL::PixelStoreParameter::unpack_alignment, alignment);

	glTexSubImage2D(d->target,0,xoffset,yoffset,width,height,d->dataFormat,d->datatype,data);
	glGenerateMipmap(d->target);
	XOpenGLFuntion::xglPixelStorei(XOpenGL::PixelStoreParameter::unpack_alignment, oldPackAlignment);
	XOpenGLFuntion::checkGLError();
}

void XOpenGLTexture::setSubData3D(int xoffset, int yoffset, int zoffset, int width, int height, int depth, const void* data)
{
	auto alignment = 1;
	auto oldPackAlignment = XOpenGLFuntion::xglPixelStorei(XOpenGL::PixelStoreParameter::unpack_alignment, alignment);
	glTexSubImage3D(d->target, 0, xoffset, yoffset, zoffset, width, height, depth, d->dataFormat, d->datatype, data);
	glGenerateMipmap(d->target);
	XOpenGLFuntion::xglPixelStorei(XOpenGL::PixelStoreParameter::unpack_alignment, oldPackAlignment);
	XOpenGLFuntion::checkGLError();
}

sptr<XUCharArray> XOpenGLTexture::getSubData1D(int xoffset, int width,int level,int layer)
{
	if (d->target == XOpenGLTexture::Target1D) {
		layer = 0;
	}

	if (XOpenGLFuntion::isSupport4_5() &&d->target ==XOpenGLTexture::Target1D) {
		auto alignment = 1;
		auto oldPackAlignment = XOpenGLFuntion::xglPixelStorei(XOpenGL::PixelStoreParameter::pack_alignment, alignment);

		auto perpixelSize = getExternalPerpixelSize(d->dataFormat, d->datatype);
		auto bufferSize = width * perpixelSize;
		auto data = makeShareDbObject<XUCharArray>();
		data->setComponent(perpixelSize);
		data->setNumOfTuple(width);
		glGetTextureSubImage(getId(), 0, xoffset, 0, 0, width, 1, 1, d->dataFormat, d->datatype, bufferSize, data->data(0));
		XOpenGLFuntion::xglPixelStorei(XOpenGL::PixelStoreParameter::pack_alignment, oldPackAlignment);
		XOpenGLFuntion::checkGLError();

		return data;
	}
	else {
		auto sourceFbo = makeShareDbObject<XOpenGLFramebufferObject>();
		sourceFbo->create();
		sourceFbo->setHeight(1);
		sourceFbo->setWidth(d->width);
		sourceFbo->bind();
		auto InternalFormat = getInternalFormat();
		if (
			InternalFormat == TextureFormat::D16 ||
			InternalFormat == TextureFormat::D24 ||
			InternalFormat == TextureFormat::D32 ||
			InternalFormat == TextureFormat::D32F) {
			sourceFbo->addAttachment(XOpenGLFramebufferObject::Attachment::Depth, asDerived<XOpenGLTexture>(), 0, level, layer);
		}
		else if (
			InternalFormat == TextureFormat::S8) {
			sourceFbo->addAttachment(XOpenGLFramebufferObject::Attachment::stencil, asDerived<XOpenGLTexture>(), 0, level, layer);
		}
		else if (
			InternalFormat == TextureFormat::D24S8) {
			sourceFbo->addAttachment(XOpenGLFramebufferObject::Attachment::CombinedDepthStencil, asDerived<XOpenGLTexture>(), 0, level, layer);
		}
		else {
			sourceFbo->addAttachment(XOpenGLFramebufferObject::Attachment::Color, asDerived<XOpenGLTexture>(), 0, level, layer);
		}

		bool flag = sourceFbo->isComplete();
		sourceFbo->release();

		sourceFbo->bind(XOpenGL::FrameBufferType::readBuffer);
		auto perpixelSize = getExternalPerpixelSize(d->dataFormat, d->datatype);
		auto data = makeShareDbObject<XUCharArray>();
		data->setComponent(perpixelSize);
		data->setNumOfTuple(width);
		XOpenGLFuntion::xglReadPixels(xoffset, 0, width, 1, (XOpenGL::TextureExternalFormat)d->dataFormat, (XOpenGL::DataType)d->datatype, data->data(0));
		sourceFbo->release(XOpenGL::FrameBufferType::readBuffer);
		return data;
	}
}

sptr<XUCharArray2D> XOpenGLTexture::getSubData2D(int xoffset, int yoffset, int width, int height, int level, int layer)
{
	if (d->target == XOpenGLTexture::Target2D) {
		layer = 0;
	}
	if (!XOpenGLFuntion::isSupport4_5() && d->target == XOpenGLTexture::Target1DArray) {
		return {};
	}

	if (XOpenGLFuntion::isSupport4_5() &&(d->target ==XOpenGLTexture::Target2D || d->target ==XOpenGLTexture::Target1DArray)) {
		auto alignment = 1;
		auto oldPackAlignment = XOpenGLFuntion::xglPixelStorei(XOpenGL::PixelStoreParameter::pack_alignment, alignment);
		auto perpixelSize = getExternalPerpixelSize(d->dataFormat, d->datatype);
		auto bufferSize = width *height* perpixelSize;
		auto data = makeShareDbObject<XUCharArray2D>();
		data->setComponent(perpixelSize);
		data->setDimensions(width,height);
		glGetTextureSubImage(getId(), 0, xoffset, yoffset, 0, width, height, 1, d->dataFormat, d->datatype, bufferSize, data->data(0));
		XOpenGLFuntion::xglPixelStorei(XOpenGL::PixelStoreParameter::pack_alignment, oldPackAlignment);
		XOpenGLFuntion::checkGLError();
		return data;
	}
	else {
		auto sourceFbo = makeShareDbObject<XOpenGLFramebufferObject>();
		sourceFbo->create();
		sourceFbo->setHeight(d->height);
		sourceFbo->setWidth(d->width);
		sourceFbo->bind();
		auto InternalFormat = getInternalFormat();
		if (
			InternalFormat == TextureFormat::D16 ||
			InternalFormat == TextureFormat::D24 ||
			InternalFormat == TextureFormat::D32 ||
			InternalFormat == TextureFormat::D32F) {
			sourceFbo->addAttachment(XOpenGLFramebufferObject::Attachment::Depth, asDerived<XOpenGLTexture>(), 0, level, layer);
		}
		else if (
			InternalFormat == TextureFormat::S8) {
			sourceFbo->addAttachment(XOpenGLFramebufferObject::Attachment::stencil, asDerived<XOpenGLTexture>(), 0, level, layer);
		}
		else if (
			InternalFormat == TextureFormat::D24S8) {
			sourceFbo->addAttachment(XOpenGLFramebufferObject::Attachment::CombinedDepthStencil, asDerived<XOpenGLTexture>(), 0, level, layer);
		}
		else {
			sourceFbo->addAttachment(XOpenGLFramebufferObject::Attachment::Color, asDerived<XOpenGLTexture>(), 0, level, layer);
		}

		bool flag = sourceFbo->isComplete();
		sourceFbo->release();

		sourceFbo->bind(XOpenGL::FrameBufferType::readBuffer);
		auto perpixelSize = getExternalPerpixelSize(d->dataFormat, d->datatype);
		auto data = makeShareDbObject<XUCharArray2D>();

		data->setComponent(perpixelSize);
		data->setDimensions(width, height);

		XOpenGLFuntion::xglReadPixels(xoffset, yoffset, width, height, (XOpenGL::TextureExternalFormat)d->dataFormat, (XOpenGL::DataType)d->datatype, data->data(0));
		sourceFbo->release(XOpenGL::FrameBufferType::readBuffer);
		return data;
	}
}

sptr<XUCharArray3D> XOpenGLTexture::getSubData3D(int xoffset, int yoffset, int zoffset, int width, int height, int depth)
{
	if (XOpenGLFuntion::isSupport4_5()) {
		auto perpixelSize = getExternalPerpixelSize(d->dataFormat, d->datatype);
		auto bufferSize = width*height*depth*perpixelSize;
		auto data = makeShareDbObject<XUCharArray3D>();
		data->setComponent(perpixelSize);
		data->setDimensions(width,height,depth);
		glGetTextureSubImage(getId(),0, xoffset,yoffset,zoffset,width,height,depth, d->dataFormat, d->datatype, bufferSize,data->data());
		XOpenGLFuntion::checkGLError();
		return data;
	}
	
	return {};
}

sptr<XOpenGLBuffer> XOpenGLTexture::map2pbo()
{
	//设值对齐
	auto alignment = 1;
	auto pixelSize =getExternalPerpixelSize(d->dataFormat,d->datatype);
	
	auto oldPackAlignment = XOpenGLFuntion::xglPixelStorei(XOpenGL::PixelStoreParameter::pack_alignment, alignment);

	auto pbo = makeShareDbObject<XOpenGLBuffer>();
	pbo->setBufferType(XOpenGLBuffer::PixelPackBuffer);
	pbo->setUsagePattern(XOpenGLBuffer::UsagePattern::StreamRead);
	pbo->create();
	pbo->bind();

	auto expactedSize = d->height * d->layer * d->width * pixelSize;
	auto pboSize = pbo->bufferSize();

	if (pboSize != expactedSize) {
		pbo->allocate(expactedSize);
	}

	bind();
	//glGetTextureSubImage
	glGetTexImage(
		d->target,							// 纹理目标
		0,											// mipmap级别
		d->dataFormat,					// 格式（必须匹配纹理的内部格式）
		d->datatype,						// 类型（必须匹配纹理的数据类型）
		0											// 偏移量（使用PBO时设为0）
	);
	


	XOpenGLFuntion::checkGLError();

	release();
	oldPackAlignment = XOpenGLFuntion::xglPixelStorei(XOpenGL::PixelStoreParameter::pack_alignment, oldPackAlignment);
	return pbo;
}

sptr<XOpenGLBuffer> XOpenGLTexture::mapMultiSample2pbo(unsigned int attachment)
{
	auto alignment = 1;
	auto originRowlen = d->width * getInternalFormatSize(d->internalFormat);
	auto aligmentLen = alignment == 0 ? originRowlen : ((originRowlen + alignment - 1) / alignment) * alignment;


	auto sourceFbo = makeShareDbObject<XOpenGLFramebufferObject>();
	sourceFbo->create();
	sourceFbo->setHeight(d->height);
	sourceFbo->setWidth(d->width);
	sourceFbo->bind();
	sourceFbo->addAttachment((XOpenGLFramebufferObject::Attachment)attachment,asDerived<XOpenGLTexture>(),0);
	bool flag = sourceFbo->isComplete();
	sourceFbo->release();

	auto resoveFbo = makeShareDbObject<XOpenGLFramebufferObject>();
	resoveFbo->create();
	resoveFbo->setHeight(d->height);
	resoveFbo->setWidth(d->width);
	resoveFbo->bind();
	resoveFbo->addAttachment((XOpenGLFramebufferObject::Attachment)attachment, d->internalFormat, d->dataFormat, d->datatype, 0);
	bool flag2 = resoveFbo->isComplete();
	resoveFbo->release();

	XOpenGLFuntion::xglBindFramebuffer(XOpenGL::FrameBufferType::readBuffer, sourceFbo->getId());
	XOpenGLFuntion::xglBindFramebuffer(XOpenGL::FrameBufferType::drawBuffer, resoveFbo->getId());

	XOpenGLFuntion::xglBlitFramebuffer(0, 0, d->width, d->height, 0, 0, d->width, d->height,
		XOpenGL::FlagBits::color_buffer_bit, XOpenGL::FilterType::nearest);

	if (attachment == XOpenGLFramebufferObject::Attachment::Color) {
		auto texture = resoveFbo->getColorAttachment();
		return texture->map2pbo();
	}else {
		auto texture = resoveFbo->getDepthAttachment();
		return texture->map2pbo();
	}
}


XOpenGLTexture::Target XOpenGLTexture::getTarget() const
{
	return d->target;
}

uint32_t XOpenGLTexture::getId() const
{
	return d->textureId;
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

std::string to_hex(unsigned int value)  {
	std::ostringstream oss;
	oss << "0x" << std::setfill('0') << std::setw(4) << std::hex << std::uppercase << value;
	return oss.str();
	};

std::string XOpenGLTexture::dump()
{
	nlohmann::json obj;
	obj["textureId"] = d->textureId;
	obj["target"] = to_hex(d->target);
	obj["width"] = d->width;
	obj["height"] = d->height;
	obj["depth"] = d->layer;
	obj["internalFormat"] = to_hex(d->internalFormat);
	obj["dataFormat"] = to_hex(d->dataFormat);
	obj["datatype"] = to_hex(d->datatype);
	obj["samples"] = d->samples;
	return obj.dump(2);
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

unsigned int XOpenGLTexture::getExternalPerpixelSize(XOpenGLTexture::PixelFormat format, XOpenGLTexture::PixelType type)
{
	auto comoponentSize = 1;
	auto byteNum =1;		
	switch (format)
	{
	case XOpenGLTexture::PixelFormat::NoSourceFormat:
		break;
	case XOpenGLTexture::PixelFormat::Red:
		comoponentSize = 1;
		break;
	case XOpenGLTexture::PixelFormat::RG:
		comoponentSize = 2;
		break;
	case XOpenGLTexture::PixelFormat::RGB:
		comoponentSize = 3;
		break;
	case XOpenGLTexture::PixelFormat::BGR:
		comoponentSize = 3;
		break;
	case XOpenGLTexture::PixelFormat::RGBA:
		comoponentSize = 4;
		break;
	case XOpenGLTexture::PixelFormat::BGRA:
		comoponentSize = 4;
		break;
	case XOpenGLTexture::PixelFormat::Red_Integer:
		comoponentSize = 1;
		break;
	case XOpenGLTexture::PixelFormat::RG_Integer:
		comoponentSize = 2;
		break;
	case XOpenGLTexture::PixelFormat::RGB_Integer:
		comoponentSize = 3;
		break;
	case XOpenGLTexture::PixelFormat::BGR_Integer:
		comoponentSize = 3;
		break;
	case XOpenGLTexture::PixelFormat::RGBA_Integer:
		comoponentSize = 4;
		break;
	case XOpenGLTexture::PixelFormat::BGRA_Integer:
		comoponentSize = 4;
		break;
	case XOpenGLTexture::PixelFormat::Stencil:
		comoponentSize = 1;
		break;
	case XOpenGLTexture::PixelFormat::Depth:
		comoponentSize = 4;
		break;
	case XOpenGLTexture::PixelFormat::DepthStencil:
		comoponentSize = 4;
		break;
	case XOpenGLTexture::PixelFormat::Alpha:
		break;
	case XOpenGLTexture::PixelFormat::Luminance:
		break;
	case XOpenGLTexture::PixelFormat::LuminanceAlpha:
		break;
	default:
		break;
	}

	switch (type)
	{
	case XOpenGLTexture::PixelType::NoPixelType:
		break;
	case XOpenGLTexture::PixelType::Int8:
		byteNum =1;
		break;
	case XOpenGLTexture::PixelType::UInt8:
		byteNum = 1;
		break;
	case XOpenGLTexture::PixelType::Int16:
		byteNum = 2;
		break;
	case XOpenGLTexture::PixelType::UInt16:
		byteNum = 2;
		break;
	case XOpenGLTexture::PixelType::Int32:
		byteNum = 4;
		break;
	case XOpenGLTexture::PixelType::UInt32:
		byteNum = 4;
		break;
	case XOpenGLTexture::PixelType::Float16:
		byteNum = 2;
		break;
	case XOpenGLTexture::PixelType::Float16OES:
		byteNum = 2;
		break;
	case XOpenGLTexture::PixelType::Float32:
		byteNum = 4;
		break;
	default:
		break;
	}
	return comoponentSize*byteNum;
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

int XOpenGLTexture::getStorgeSize()
{
	auto dim = *XOpenGLFuntion::xGetTextureSize(d->textureId,(XOpenGL::TextureTarget)d->target);
	auto pixelSize = getInternalFormatSize(d->internalFormat);
	return dim[0]*dim[1]*dim[2]*pixelSize;
}

int XOpenGLTexture::computeStorgeSize(int width, int height, int depth, XOpenGLTexture::TextureFormat format, int sampleNum)
{
	return sampleNum*width*height*depth*getInternalFormatSize(format);
}