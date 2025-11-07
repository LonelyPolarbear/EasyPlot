#include <map>
#include "XOpenGLVertexArrayObject.h"
#include "glew/glew.h"

#include "XOpenGLBuffer.h"

class XOpenGLVertexArrayObject::Internal
{
public:
    GLuint m_id{0};
    std::map<unsigned int,std::shared_ptr<XOpenGLBuffer>> m_buffers;
};

XOpenGLVertexArrayObject::XOpenGLVertexArrayObject():
    d(new Internal)
{
}

XOpenGLVertexArrayObject::~XOpenGLVertexArrayObject()
{
    destroy();
}

void XOpenGLVertexArrayObject::create()
{
    glGenVertexArrays(1, &d->m_id);
}

void XOpenGLVertexArrayObject::destroy()
{
    glDeleteVertexArrays(1, &d->m_id);
}

void XOpenGLVertexArrayObject::bind()
{
    glBindVertexArray(d->m_id );
}

void XOpenGLVertexArrayObject::release()
{
    glBindVertexArray(0);
}

void XOpenGLVertexArrayObject::addBuffer(unsigned int index, 
std::shared_ptr<XOpenGLBuffer> buffer, 
unsigned int size, 
XOpenGL::DataType type,
unsigned int stride, 
unsigned int offset,
 bool isInstance)
{
    bind();
    buffer->bind();
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, size, (unsigned int)type, GL_FALSE, stride, (void*)offset);
    if (isInstance) {
        glVertexAttribDivisor(index, 1);
    }
    d->m_buffers[index] = buffer;
    buffer->release();
    release();
}
