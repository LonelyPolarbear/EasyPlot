#include <glew/glew.h>
#include<windows.h>

#include "XOpenGLWidget.h"
#include <QOpenGLContext>
#include <QWindow>
#include <QResizeEvent>

#include <QDebug>
#include <iostream>
#include <chrono>

#include "lib04_opengl/XOpenGLContext.h"

using namespace std::chrono_literals;

#define WGL_CONTEXT_MAJOR_VERSION_ARB  0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB  0x2092
#define WGL_CONTEXT_PROFILE_MASK_ARB   0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB 0x00000001
#define WGL_CONTEXT_FLAGS_ARB 0x00000002
#define WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB 0x2094

// WGL_ARB_pixel_format扩展宏定义
#define WGL_DRAW_TO_WINDOW_ARB             0x2001
#define WGL_DRAW_TO_BITMAP_ARB             0x2002
#define WGL_ACCELERATION_ARB               0x2003
#define WGL_NEED_PALETTE_ARB               0x2004
#define WGL_NEED_SYSTEM_PALETTE_ARB        0x2005
#define WGL_SWAP_LAYER_BUFFERS_ARB         0x2006
#define WGL_SWAP_METHOD_ARB                0x2007
#define WGL_NUMBER_OVERLAYS_ARB            0x2008
#define WGL_NUMBER_UNDERLAYS_ARB           0x2009
#define WGL_TRANSPARENT_ARB                0x200A
#define WGL_TRANSPARENT_RED_VALUE_ARB      0x2037
#define WGL_TRANSPARENT_GREEN_VALUE_ARB    0x2038
#define WGL_TRANSPARENT_BLUE_VALUE_ARB     0x2039
#define WGL_TRANSPARENT_ALPHA_VALUE_ARB    0x203A
#define WGL_TRANSPARENT_INDEX_VALUE_ARB    0x203B
#define WGL_SHARE_DEPTH_ARB                0x200C
#define WGL_SHARE_STENCIL_ARB              0x200D
#define WGL_SHARE_ACCUM_ARB                0x200E
#define WGL_SUPPORT_GDI_ARB                0x200F
#define WGL_SUPPORT_OPENGL_ARB             0x2010
#define WGL_DOUBLE_BUFFER_ARB              0x2011
#define WGL_STEREO_ARB                     0x2012
#define WGL_PIXEL_TYPE_ARB                 0x2013
#define WGL_COLOR_BITS_ARB                 0x2014
#define WGL_RED_BITS_ARB                   0x2015
#define WGL_RED_SHIFT_ARB                  0x2016
#define WGL_GREEN_BITS_ARB                 0x2017
#define WGL_GREEN_SHIFT_ARB                0x2018
#define WGL_BLUE_BITS_ARB                  0x2019
#define WGL_BLUE_SHIFT_ARB                 0x201A
#define WGL_ALPHA_BITS_ARB                 0x201B
#define WGL_ALPHA_SHIFT_ARB                0x201C
#define WGL_ACCUM_BITS_ARB                 0x201D
#define WGL_ACCUM_RED_BITS_ARB             0x201E
#define WGL_ACCUM_GREEN_BITS_ARB           0x201F
#define WGL_ACCUM_BLUE_BITS_ARB            0x2020
#define WGL_ACCUM_ALPHA_BITS_ARB           0x2021
#define WGL_DEPTH_BITS_ARB                 0x2022
#define WGL_STENCIL_BITS_ARB               0x2023
#define WGL_AUX_BUFFERS_ARB                0x2024
#define WGL_NO_ACCELERATION_ARB            0x2025
#define WGL_GENERIC_ACCELERATION_ARB       0x2026
#define WGL_FULL_ACCELERATION_ARB          0x2027
#define WGL_SWAP_EXCHANGE_ARB              0x2028
#define WGL_SWAP_COPY_ARB                  0x2029
#define WGL_SWAP_UNDEFINED_ARB             0x202A
#define WGL_TYPE_RGBA_ARB                  0x202B
#define WGL_TYPE_COLORINDEX_ARB            0x202C
#define WGL_SAMPLE_BUFFERS_ARB             0x2041  // 多重采样缓冲开关
#define WGL_SAMPLES_ARB                    0x2042  // 采样点数（如4x MSAA）


typedef HGLRC(WINAPI* PFNWGLCREATECONTEXTATTRIBSARBPROC) (HDC hDC, HGLRC hShareContext, const int* attribList);
typedef BOOL(WINAPI* PFNWGLCHOOSEPIXELFORMATARBPROC)(HDC hdc, const int* piAttribList, const FLOAT* pfAttribFList, UINT nMaxFormats, int* piFormats, UINT* nNumFormats);
PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;

XOpenGLWidget::XOpenGLWidget(QWidget* parent) :QWidget(parent)
{
	// 设置 Widget 属性
	setAttribute(Qt::WA_PaintOnScreen, true); // 禁用 Qt 的绘制缓冲
	setAttribute(Qt::WA_NativeWindow, true);	// 强制使用原生窗口句柄
	setAttribute(Qt::WA_NoSystemBackground, true);
	setAttribute(Qt::WA_OpaquePaintEvent, true);

	this->startTimer(60ms);

	mContext = makeShareDbObject<XOpenGLContext>();
}

XOpenGLWidget::~XOpenGLWidget()
{
	makeCurrent();
	doneCurrent();
}

bool XOpenGLWidget::initOpenglContext()
{

	HDC hdc = GetDC((HWND)winId());
	if(!createTempContext()){
		return false;
	}

	// 多重采样像素格式属性列表
	const int pixelAttribs[] = {
		// 基础属性
		WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,        // 可绘制到窗口
		WGL_SUPPORT_OPENGL_ARB, GL_TRUE,        // 支持OpenGL
		WGL_DOUBLE_BUFFER_ARB, GL_TRUE,         // 双缓冲
		WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,  // RGBA颜色类型
		WGL_COLOR_BITS_ARB, 32,                 // 32位颜色深度
		WGL_DEPTH_BITS_ARB, 24,                 // 24位深度缓冲
		WGL_STENCIL_BITS_ARB, 8,                // 8位模板缓冲

		// 多重采样关键属性
		WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,        // 启用采样缓冲区（必须）
		WGL_SAMPLES_ARB, 8,                     // 4x MSAA（采样点数，可修改为2、8等）
		0  // 属性列表结束标记
	};

	UINT numFormats;
	int pixelFormat2;
	if (!wglChoosePixelFormatARB(hdc, pixelAttribs, NULL, 1, &pixelFormat2, &numFormats) || numFormats == 0) {
		return false;
	}

	ReleaseDC((HWND)winId(), hdc);
	hdc = GetDC((HWND)winId()); // 重新获取HDC

	// 设置像素格式
	PIXELFORMATDESCRIPTOR pfd2;
	DescribePixelFormat(hdc, pixelFormat2, sizeof(pfd2), &pfd2);
	if (!SetPixelFormat(hdc, pixelFormat2, &pfd2)) {
		return false;
	}

	const int attribs[] = {
			WGL_CONTEXT_MAJOR_VERSION_ARB, 4, // OpenGL 4.6
			WGL_CONTEXT_MINOR_VERSION_ARB, 6,
			WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
			//WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
			0
	};

	HGLRC modernContext = wglCreateContextAttribsARB(hdc, 0, attribs);
	// 激活现代上下文
	wglMakeCurrent(hdc, modernContext);

	// 保存上下文信息
	//nativeContext = modernContext;
	//nativeDisplay = hdc;

	mContext->setNativeContext(modernContext);
	mContext->setNativeDisplay(hdc);

	// 初始化GLEW
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		qDebug() << "GLEW Error:" << glewGetErrorString(glewInit());
	}
	std::cout << glGetString(GL_VERSION) << std::endl;

	// 检查实际上下文类型
	int context_flags;
	glGetIntegerv(GL_CONTEXT_FLAGS, &context_flags);

	int profile_mask;
	glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &profile_mask);

	std::cout << "Context flags: 0x" << std::hex << context_flags << "\n";
	std::cout << "Profile mask: 0x" << profile_mask << "\n";

	// 检查是否核心模式
	if (profile_mask & GL_CONTEXT_CORE_PROFILE_BIT) {
		std::cout << "Core Profile Context\n";
	}
	else if (profile_mask & GL_CONTEXT_COMPATIBILITY_PROFILE_BIT) {
		std::cout << "Compatibility Profile Context\n";
	}

	// 检查向前兼容标志
	if (context_flags & GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT) {
		std::cout << "Forward Compatible\n";
	}

	// 检查调试标志
	if (context_flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
		std::cout << "Debug Context\n";
	}
}


WId XOpenGLWidget::nativeHandle()
{
	return winId();
}

bool XOpenGLWidget::makeCurrent()
{
	//return wglMakeCurrent((HDC)nativeDisplay, (HGLRC)nativeContext);

	return mContext->makeCurrent();
}

void XOpenGLWidget::doneCurrent()
{
	wglMakeCurrent(nullptr, nullptr);
	//wglDeleteContext((HGLRC)nativeContext);
	//ReleaseDC((HWND)winId(), (HDC)nativeDisplay);
}

void XOpenGLWidget::swapBuffers()
{
	//SwapBuffers((HDC)nativeDisplay);
	return mContext->swapBuffers();
}


void XOpenGLWidget::resizeEvent(QResizeEvent* event)
{
	// 窗口大小改变事件处理
	mWidth = event->size().width();
	mHeight = event->size().height();
	if (m_isInit) {
		makeCurrent();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, event->size().width(), event->size().height());
		doneCurrent();
	}
	return QWidget::resizeEvent(event);
}

void XOpenGLWidget::timerEvent(QTimerEvent* event)
{
	// 定时器事件处理
	if (!m_isInit)
		return;

	render();
}

void XOpenGLWidget::showEvent(QShowEvent* event)
{
	if (m_isInit == false) {
		//if (testAttribute(Qt::WA_WState_Created)) {
		initOpenglContext();
		m_isInit = true;
		initGLResource();
		//}
	}
	return QWidget::showEvent(event);
}

//QWidget::paintEngine: Should no longer be called
QPaintEngine* XOpenGLWidget::paintEngine() const
{
	return nullptr;
}

void XOpenGLWidget::render()
{
	makeCurrent();
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0.2f, 0.0f, 0.8f, 1.0f);

	swapBuffers();
	glUseProgram(0);
	doneCurrent();
}

bool XOpenGLWidget::createTempContext()
{
	//HDC hdc = GetDC((HWND)winId());
	WNDCLASS wc = { 0 };
	wc.lpfnWndProc = DefWindowProc;
	wc.hInstance = GetModuleHandle(NULL);
	wc.lpszClassName = "TempGLWindow";

	if (!RegisterClass(&wc)) return false;

	auto tempHWnd = CreateWindowEx(0, "TempGLWindow", "Temp Window", 0,
		0, 0, 1, 1, NULL, NULL, GetModuleHandle(NULL), NULL);
	if (!tempHWnd) return false;

	HDC hdc = GetDC(tempHWnd);

	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		32, // 颜色深度
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0,
		24, // 深度缓冲
		8,  // 模板缓冲
		0,
		PFD_MAIN_PLANE,
		0, 0, 0, 0
	};


	int pixelFormat = ChoosePixelFormat(hdc, &pfd);
	if (!pixelFormat) return false;

	if (!SetPixelFormat(hdc, pixelFormat, &pfd)) return false;

	HGLRC tempContext = wglCreateContext(hdc);
	if (!tempContext) return false;

	if (!wglMakeCurrent(hdc, tempContext)) return false;


	wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
	wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");

	// 删除临时上下文
	wglMakeCurrent(nullptr, nullptr);
	wglDeleteContext(tempContext);

	ReleaseDC(tempHWnd, hdc);
	DestroyWindow(tempHWnd);
	UnregisterClass("TempGLWindow", GetModuleHandle(NULL));
	return true;
}

std::shared_ptr<XOpenGLContext> XOpenGLWidget::getContext() const{
	return mContext;
}

myUtilty::Vec2u  XOpenGLWidget::mapToGLScreen(const QPoint& point) {
	myUtilty::Vec2u p;
	p.x =point.x();
	p.y =mHeight- point.y();
	return p;
}

myUtilty::Vec2f  XOpenGLWidget::mapToNormGLScreen(const QPoint& point) {
	myUtilty::Vec2f p;
	p.x =(float)point.x() /(float) mWidth;
	p.y =(float)(mHeight - point.y())/(float) mHeight;
	return p;
}
