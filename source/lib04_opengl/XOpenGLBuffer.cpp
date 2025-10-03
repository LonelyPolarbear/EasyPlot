#include "XOpenGLBuffer.h"
#include <glew/glew.h>
class XOpenGLBuffer::Internal {
public:
	XOpenGLBuffer::Type type;
	XOpenGLBuffer::UsagePattern usagePattern;
    GLuint bufferId;
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
	void *result = nullptr;
	result = glMapBuffer(d->type, access);
	return nullptr;
}

bool XOpenGLBuffer::unmap()
{
	return glUnmapBuffer(d->type) == GL_TRUE;
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

bool XOpenGLBuffer::bind()
{
	glBindBuffer(d->type, d->bufferId);
	return glGetError() == GL_NO_ERROR;
}

void XOpenGLBuffer::release()
{
	glBindBuffer(d->type, 0);
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
