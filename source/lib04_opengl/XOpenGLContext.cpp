#include <glew/glew.h>
#include<windows.h>
#include "XOpenGLContext.h"

XOpenGLContext::XOpenGLContext()
{
}

XOpenGLContext::~XOpenGLContext()
{
}

bool XOpenGLContext::makeCurrent()
{
	return wglMakeCurrent((HDC)nativeDisplay, (HGLRC)nativeContext);
}

void XOpenGLContext::doneCurrent()
{
	wglMakeCurrent(nullptr, nullptr);
	//wglDeleteContext((HGLRC)nativeContext);
	//ReleaseDC((HWND)winId(), (HDC)nativeDisplay);
}

void XOpenGLContext::swapBuffers()
{
	SwapBuffers((HDC)nativeDisplay);
}

void XOpenGLContext::setNativeContext(void* context)
{
	nativeContext = context;
}

void XOpenGLContext::setNativeDisplay(void* display)
{
	nativeDisplay = display;
}

void* XOpenGLContext::getNativeContext() const
{
	return nativeContext;
}

void* XOpenGLContext::getNativeDisplay() const
{
	return nativeDisplay;
}

bool XOpenGLContext::isValid()
{
	return nativeDisplay && nativeContext;
}
