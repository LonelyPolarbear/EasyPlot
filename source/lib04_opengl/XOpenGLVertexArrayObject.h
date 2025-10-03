#pragma once

#include "XOpenglApi.h"
#include <dataBase/dataobject.h>

class XOpenGLBuffer;
class LIB04_OPENGL_API XOpenGLVertexArrayObject:DataBaseObject
{
protected:
    XOpenGLVertexArrayObject();
    ~XOpenGLVertexArrayObject();
 public:
    void create();

    void destroy();

    void bind();

	void release();

    /// <summary>
    /// 
    /// </summary>
    /// <param name="index">顶点属性指针</param>
    /// <param name="buffer">该顶点关联的VBO</param>
    /// <param name="size"></param>
    /// <param name="type"></param>
    /// <param name="stride">步长</param>
    /// <param name="offset">偏移量</param>
    void addBuffer(unsigned int index, std::shared_ptr<XOpenGLBuffer> buffer, unsigned int size, unsigned int type, unsigned int stride, unsigned int offset,bool isInstance=false);
    public:
    class Internal;
    std::unique_ptr<Internal> d;
};