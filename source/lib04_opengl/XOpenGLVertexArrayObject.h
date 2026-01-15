#pragma once

#include "XOpenglApi.h"
#include <dataBase/XDataBaseObject.h>
#include "XOpenGLType.h"

class XOpenGLBuffer;
class LIB04_OPENGL_API XOpenGLVertexArrayObject:public XDataBaseObject
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
    /// 绑定顶点属性
    /// </summary>
    /// <param name="index">顶点属性指针</param>
    /// <param name="buffer">该顶点关联的VBO</param>
    /// <param name="size">该属性是几个type</param>
    /// <param name="type">属性的数据类型</param>
    /// <param name="stride">步长，单位字节</param>
    /// <param name="offset">偏移量，单位字节</param>
    void addBuffer(unsigned int index, std::shared_ptr<XOpenGLBuffer> buffer, unsigned int size, XOpenGL::DataType type, unsigned int stride, unsigned int offset,bool isInstance=false);
    public:
    class Internal;
    std::unique_ptr<Internal> d;
};