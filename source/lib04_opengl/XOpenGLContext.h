#pragma once
#include "XOpenglApi.h"
#include <dataBase/XDataBaseObject.h>

class LIB04_OPENGL_API XOpenGLShareContext :public XDataBaseObject {
protected:
	XOpenGLShareContext();
	virtual ~XOpenGLShareContext();
public:
	bool makeCurrent();

	void doneCurrent();

	void setNativeContext(void* context);

	void setNativeDisplay(void* display);

	void* getNativeContext() const;

	bool isValid();
private:
	//平台原生上下文
	void* nativeContext = nullptr;
	void* nativeDisplay = nullptr;
};

class LIB04_OPENGL_API XOpenGLContext :public XDataBaseObject {
protected:
	XOpenGLContext();
	virtual ~XOpenGLContext();
public:
	bool makeCurrent();

	void doneCurrent();

	void swapBuffers();

	void* getNativeContext() const;

	void* getNativeDisplay() const;

	bool isValid();

	bool create(uint64_t window);

	std::shared_ptr<XOpenGLShareContext> createOrgetShareContext();

protected:
	bool createTempContext();
	void setNativeContext(void* context);
	void setNativeDisplay(void* display);
	//平台原生上下文
	void* nativeContext = nullptr;
	void* nativeDisplay = nullptr;

	std::shared_ptr<XOpenGLShareContext> shareContext;
};