#pragma once
#include "XOpenglApi.h"
#include <dataBase/dataobject.h>

//class ISResultBaseAPI ISPostObject : public std::enable_shared_from_this<ISPostObject>
class LIB04_OPENGL_API XOpenGLContext :public DataBaseObject {
protected:
	XOpenGLContext();
	virtual ~XOpenGLContext();
public:
	bool makeCurrent();

	void doneCurrent();

	void swapBuffers();

	void setNativeContext(void* context);
	void setNativeDisplay(void* display);

	void* getNativeContext() const;
	void* getNativeDisplay() const;

	bool isValid();
private:
	//平台原生上下文
	void* nativeContext = nullptr;
	void* nativeDisplay = nullptr;
};