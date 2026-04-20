#include <glew/glew.h>
#include<windows.h>
#include <glew/wglew.h>

#include "XOpenGLContext.h"
#include <iostream>
#include <xlog/XLogger.h>

//回调函数
void GLAPIENTRY debugCallback(GLenum source, GLenum type, GLuint id,
	GLenum severity, GLsizei length,
	const GLchar* message, const void* userParam) {
	// 忽略非错误的通知性消息，减少控制台输出干扰
	if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
		return;

	/*fprintf(stderr, "GL Debug [Source: 0x%x, Type: 0x%x, Severity: 0x%x]: %s\n",
		source, type, severity, message);*/

	XLOG_ERROR("GL Debug [Source:{}, Type:{}, Severity:{}]: {}", source, type, severity, message);
}

HGLRC CreateWindowlessContext(HGLRC shareContext, HWND& tempWindow) {
	// 使用wglCreateContextAttribsARB创建现代OpenGL上下文
	HDC tempDC = nullptr;
	//HWND tempWindow = nullptr;
	HGLRC tempContext = nullptr;
	HGLRC finalContext = nullptr;

	do {
		// 创建临时窗口用于获取DC
		tempWindow = CreateWindowEx(
			0, "Static", "Temp", WS_POPUP,
			0, 0, 1, 1, nullptr, nullptr, GetModuleHandle(nullptr), nullptr);
		if (!tempWindow) break;

		tempDC = GetDC(tempWindow);
		if (!tempDC) break;

		// 设置临时的像素格式
		PIXELFORMATDESCRIPTOR tempPfd = {
			sizeof(PIXELFORMATDESCRIPTOR), 1,
			PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
			PFD_TYPE_RGBA, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			24, 8, 0, 0, 0, 0, 0, 0
		};

		int tempFormat = ChoosePixelFormat(tempDC, &tempPfd);
		if (!tempFormat) break;

		if (!SetPixelFormat(tempDC, tempFormat, &tempPfd)) break;

		// 创建临时上下文
		tempContext = wglCreateContext(tempDC);
		if (!tempContext) break;

		if (!wglMakeCurrent(tempDC, tempContext)) break;

		// 初始化GLEW
		glewExperimental = GL_TRUE;
		if (glewInit() != GLEW_OK) {
			//std::cerr << "无法初始化GLEW" << std::endl;
			break;
		}

		// 检查WGL_ARB_create_context扩展
		if (!WGLEW_ARB_create_context) {
			//std::cerr << "不支持WGL_ARB_create_context扩展" << std::endl;
			break;
		}

		// 使用wglCreateContextAttribsARB创建无窗口上下文
		// 关键：使用WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB而不是核心配置文件
		// 这样可以在没有默认帧缓冲区的情况下工作
		int attribs[] = {
			WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
			WGL_CONTEXT_MINOR_VERSION_ARB, 6,
			WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
			WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
			0
		};

		// 注意：这里使用tempDC，但实际上这个上下文不会与任何窗口关联
		// 一旦创建，我们可以使其在任何线程中成为当前上下文
		finalContext = wglCreateContextAttribsARB(tempDC, shareContext, attribs);
		if (!finalContext) {
			//std::cout << "无法使用wglCreateContextAttribsARB创建上下文" << std::endl;
			finalContext = wglCreateContext(tempDC);
			if (finalContext && shareContext) {
				if (!wglShareLists(shareContext, finalContext)) {
					//std::cout << "无法共享上下文列表" << std::endl;
				}
			}
		}

	} while (false);

	// 清理临时资源
	if (tempContext) {
		wglMakeCurrent(nullptr, nullptr);
		wglDeleteContext(tempContext);
	}
	if (tempDC && tempWindow) {
		//ReleaseDC(tempWindow, tempDC);
	}
	if (tempWindow) {
		//DestroyWindow(tempWindow);
	}

	return finalContext;
}

XOpenGLContext::XOpenGLContext()
{
}

XOpenGLContext::~XOpenGLContext()
{
	reset();
}

bool XOpenGLContext::makeCurrent()
{
	if(isValid())
		return wglMakeCurrent((HDC)nativeDisplay, (HGLRC)nativeContext);
	return false;
}

void XOpenGLContext::doneCurrent()
{
	if (isValid())
		wglMakeCurrent(nullptr, nullptr);
}

void XOpenGLContext::swapBuffers()
{
	if (isValid())
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

bool XOpenGLContext::createTempContext()
{
	static bool initialized = false;
	if (initialized) return true;
		initialized = true;

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

bool XOpenGLContext::create(uint64_t winId)
{
	createTempContext();

	HDC hdc = GetDC((HWND)winId);
	if (!createTempContext()) {
		return false;
	}

	// 多重采样像素格式属性列表
	const int pixelAttribs[] = {
		// 基础属性
		WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,						// 可绘制到窗口
		WGL_SUPPORT_OPENGL_ARB, GL_TRUE,							// 支持OpenGL
		WGL_DOUBLE_BUFFER_ARB, GL_TRUE,							// 双缓冲
		WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,				// RGBA颜色类型
		WGL_COLOR_BITS_ARB, 32,												// 32位颜色深度
		WGL_DEPTH_BITS_ARB, 24,												// 24位深度缓冲
		WGL_STENCIL_BITS_ARB, 8,												 // 8位模板缓冲

		// 多重采样关键属性
		WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,							// 启用采样缓冲区（必须）
		WGL_SAMPLES_ARB, 8,														 // 4x MSAA（采样点数，可修改为2、8等）
		0  // 属性列表结束标记
	};

	UINT numFormats;
	int pixelFormat2;
	if (!wglChoosePixelFormatARB(hdc, pixelAttribs, NULL, 1, &pixelFormat2, &numFormats) || numFormats == 0) {
		return false;
	}

	ReleaseDC((HWND)winId, hdc);
	hdc = GetDC((HWND)winId); // 重新获取HDC

	// 设置像素格式
	PIXELFORMATDESCRIPTOR pfd2;
	DescribePixelFormat(hdc, pixelFormat2, sizeof(pfd2), &pfd2);
	if (!SetPixelFormat(hdc, pixelFormat2, &pfd2)) {
		return false;
	}

	const int attribs[] = {
			WGL_CONTEXT_MAJOR_VERSION_ARB, 4,																						// 主版本号
			WGL_CONTEXT_MINOR_VERSION_ARB, 6,																						// 次版本号
			WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,						// 核心模式
			WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,													//开启调试
			0																																							//结束标记
	};

	HGLRC modernContext = wglCreateContextAttribsARB(hdc, 0, attribs);

	// 激活现代上下文
	wglMakeCurrent(hdc, modernContext);

	// 保存上下文信息
	//nativeContext = modernContext;
	//nativeDisplay = hdc;

	setNativeContext(modernContext);
	setNativeDisplay(hdc);

	// 初始化GLEW
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		//std::cout << "GLEW Error:" << glewGetErrorString(glewInit())<<std::endl;
		//std::string ss((const char*)glewGetErrorString(glewInit()));
		XLOG_ERROR("GLEW Error:",  std::string(reinterpret_cast<const char*>(glewGetErrorString(glewInit()))));
	}
	else {
		//启用调试
		glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback(debugCallback, nullptr);
	}
	
	XLOG_INFO("OpenGL version:", std::string(reinterpret_cast<const char*>(glGetString(GL_VERSION))));

	// 检查实际上下文类型
	int context_flags;
	glGetIntegerv(GL_CONTEXT_FLAGS, &context_flags);

	int profile_mask;
	glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &profile_mask);

	XLOG_INFO("Context flags:", context_flags);
	XLOG_INFO("Profile mask: ", profile_mask);

	//std::cout << "Context flags: 0x" << std::hex << context_flags << "\n";
	//std::cout << "Profile mask: 0x" << profile_mask << "\n";

	// 检查是否核心模式
	if (profile_mask & GL_CONTEXT_CORE_PROFILE_BIT) {
		XLOG_INFO("Core Profile Context");
	}
	else if (profile_mask & GL_CONTEXT_COMPATIBILITY_PROFILE_BIT) {
		XLOG_INFO("Compatibility Profile Context");
	}

	// 检查向前兼容标志
	if (context_flags & GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT) {
		XLOG_INFO("Forward Compatible");
	}

	// 检查调试标志
	if (context_flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
		XLOG_INFO("Debug Context");
	}

	return true;
}

std::shared_ptr<XOpenGLShareContext> XOpenGLContext::createOrgetShareContext()
{
	if (shareContext) {
		return shareContext;
	}
	HWND window;
	auto share_Context =CreateWindowlessContext((HGLRC)nativeContext, window);
	auto dc =GetDC(window);

	if (share_Context) {
		shareContext = makeShareDbObject<XOpenGLShareContext>();
		shareContext->setNativeContext(share_Context);
		shareContext->setNativeDisplay(dc);
		return shareContext;
	}
	else {
		return nullptr;
	}
}

void XOpenGLContext::reset()
{
	if (nativeDisplay) {
		HWND window = WindowFromDC((HDC)nativeDisplay);
		ReleaseDC(window, (HDC)nativeDisplay);
	}
	
	if(nativeContext)
		wglMakeCurrent(nullptr, nullptr);

	nativeDisplay = nullptr;
	nativeContext = nullptr;
}

XOpenGLShareContext::XOpenGLShareContext()
{

}

XOpenGLShareContext::~XOpenGLShareContext()
{
	wglDeleteContext((HGLRC)nativeContext);
	HWND window = WindowFromDC((HDC)nativeDisplay);
	ReleaseDC(window, (HDC)nativeDisplay);
	DestroyWindow(window);
}

bool XOpenGLShareContext::makeCurrent()
{
	return wglMakeCurrent((HDC)nativeDisplay, (HGLRC)nativeContext);
}

void XOpenGLShareContext::doneCurrent()
{
	wglMakeCurrent(nullptr, nullptr);
}

void XOpenGLShareContext::setNativeContext(void* context)
{
	nativeContext = context;
}

void XOpenGLShareContext::setNativeDisplay(void* display)
{
	nativeDisplay = display;
}

void* XOpenGLShareContext::getNativeContext() const
{
	return nativeContext;
}

bool XOpenGLShareContext::isValid()
{
	return nativeContext;
}
//--------------------------------------------------------------------------------------
class XOffsetWindow::Internal {
public:
	HWND hwnd = nullptr;
	WNDCLASS wc = { 0 };
};

XOffsetWindow::XOffsetWindow() :mData(new Internal)
{

}

XOffsetWindow::~XOffsetWindow()
{	
	if (mData->hwnd) {
		DestroyWindow(mData->hwnd);
		UnregisterClass("OffscreenOpenGL", mData->wc.hInstance);
	}
}

void XOffsetWindow::Init()
{
	WNDCLASS wc = { 0 };
	wc.lpfnWndProc = DefWindowProc;
	wc.hInstance = GetModuleHandle(NULL);
	wc.lpszClassName = "OffscreenOpenGL";

	if (!RegisterClass(&wc)) return;

	mData->wc = wc;

	auto hwnd = CreateWindowEx(0, "OffscreenOpenGL", "Temp Window", 0,
		0, 0, 1, 1, NULL, NULL, GetModuleHandle(NULL), NULL);
	if (!hwnd) return;
	ShowWindow(hwnd, SW_HIDE);  // 隐藏
	mData->hwnd = hwnd;
}

uint64_t XOffsetWindow::winId()
{
	return (uint64_t)mData->hwnd;
}
