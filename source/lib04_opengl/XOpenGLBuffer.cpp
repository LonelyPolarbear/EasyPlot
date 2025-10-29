#include "XOpenGLBuffer.h"
#include <glew/glew.h>
#include "XOpenGLFuntion.h"

class XOpenGLBuffer::Internal {
public:
	XOpenGLBuffer::Type type;
	XOpenGLBuffer::UsagePattern usagePattern;
    GLuint bufferId;
	GLint lastBindBufferId{0};
	bool isCreated = false;
};

XOpenGLBuffer::XOpenGLBuffer():d(new Internal)
{
}

XOpenGLBuffer::~XOpenGLBuffer()
{
	destroy();
}

void XOpenGLBuffer::setBufferType(XOpenGLBuffer::Type type)
{
	d->type = type;
}

bool XOpenGLBuffer::create()
{
	glGenBuffers(1, &d->bufferId);
	d->isCreated = glGetError() == GL_NO_ERROR;
	return d->isCreated;
}

bool XOpenGLBuffer::allocate(const void* data, int count)
{
	//glBufferData(GL_ARRAY_BUFFER, count, data, GL_STATIC_DRAW);
	glBufferData(d->type, count, data, d->usagePattern);
	return glGetError() == GL_NO_ERROR;
}

bool XOpenGLBuffer::allocate(int count)
{
	glBufferData(d->type, count, nullptr, d->usagePattern);
	return glGetError() == GL_NO_ERROR;
}

uint32_t XOpenGLBuffer::bufferSize() const
{
	GLint size = 0;
	glGetBufferParameteriv(d->type, GL_BUFFER_SIZE, &size);
	return size;
}

bool XOpenGLBuffer::write(int offset, const void* data, int count)
{
	//glBufferSubData(GL_ARRAY_BUFFER, offset,count, data);
	glBufferSubData(d->type, offset, count, data);
	return glGetError() == GL_NO_ERROR;
}

bool XOpenGLBuffer::read(int offset, void*& data, int count)
{
	glGetBufferSubData(d->type, offset, count, data);
	return glGetError() == GL_NO_ERROR;
}

void* XOpenGLBuffer::map(XOpenGLBuffer::Access access)
{	
	bind();
	XOpenGLFuntion::checkGLError();
	void *result = nullptr;
	result = glMapBuffer(d->type, access);
	release();
	return result;
}

bool XOpenGLBuffer::unmap()
{
	bind();
	bool result = glUnmapBuffer(d->type) == GL_TRUE;
	release();
	return result;
}

void* XOpenGLBuffer::mapRange(int offset, int count, XOpenGLBuffer::RangeAccessFlags access)
{
	return glMapBufferRange(d->type, offset, count, access);
}

void XOpenGLBuffer::setUsagePattern(XOpenGLBuffer::UsagePattern value)
{
	d->usagePattern = value;
}

XOpenGLBuffer::UsagePattern XOpenGLBuffer::getUsagePattern() const
{
	return d->usagePattern;
}

XOpenGL::DataBufferBindingType XOpenGLBuffer::getBufferBindType() const
{
	switch (d->type)
	{
		case Type::VertexBuffer:
			return XOpenGL::DataBufferBindingType::vertexBufferBinding;
		case Type::IndexBuffer:
			return XOpenGL::DataBufferBindingType::elementArrayBufferBinding;
		case Type::UniformBuffer:
			return XOpenGL::DataBufferBindingType::uniformBufferBinding;
		case Type::PixelPackBuffer:
			return XOpenGL::DataBufferBindingType::pixelPackBufferBinding;
		case Type::PixelUnpackBuffer:
			return XOpenGL::DataBufferBindingType::pixelUnpackBufferBinding;
		case Type::ShaderStorageBuffer:
			return XOpenGL::DataBufferBindingType::shaderStorageBufferBinding;
		case Type::TransformFeedbackBuffer:
			return XOpenGL::DataBufferBindingType::transformFeedbackBufferBinding;
		default:
			return XOpenGL::DataBufferBindingType::invalidBinding;
	}
}

bool XOpenGLBuffer::bind()
{
	//应该记录上次绑定的buffer
    auto bindingType = getBufferBindType();
	int tmpLastBindBufferId =0;;
	XOpenGLFuntion::xglGetBindDataBufferId(bindingType, tmpLastBindBufferId);

	if (tmpLastBindBufferId == d->bufferId) {
		//当前已经是绑定的状态
		return true;
	}

	d->lastBindBufferId = tmpLastBindBufferId;		//记录之前绑定的buffer

	glBindBuffer(d->type, d->bufferId);

	int fffff = 0;
	XOpenGLFuntion::xglGetBindDataBufferId(bindingType, fffff);
	return glGetError() == GL_NO_ERROR;
}

void XOpenGLBuffer::release()
{
	int tmpLastBindBufferId = 0;
	auto bindingType = getBufferBindType();
	XOpenGLFuntion::xglGetBindDataBufferId(bindingType, tmpLastBindBufferId);

	//如果当前绑定的id不是自己,则不做处理
	if (tmpLastBindBufferId != d->bufferId) {
		return;
	}

	//绑定之前的id
	glBindBuffer(d->type, d->lastBindBufferId);
}

void XOpenGLBuffer::destroy()
{
	if (d->isCreated) {
		glDeleteBuffers(1, &d->bufferId);
		d->isCreated = false;
	}
}

unsigned int XOpenGLBuffer::bufferId()
{
	return d->bufferId;
}

bool XOpenGLBuffer::setBufferBindIdx(unsigned int idx)
{
	glBindBufferBase(d->type, idx, d->bufferId);
	return glGetError() == GL_NO_ERROR;
}
