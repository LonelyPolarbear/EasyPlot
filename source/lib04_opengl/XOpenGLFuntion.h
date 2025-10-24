#pragma once
#include "XOpenglApi.h"
#include "XOpenGLType.h"
#include "dataBase/dataobject.h"

class XOpenGLFramebufferObject;

/// <summary>
/// 主要用于封装OpenGL的函数，仅此而已，调用之前请确保已经初始化了OpenGL环境
/// </summary>
class LIB04_OPENGL_API XOpenGLFuntion
{
public:
	static void xglBindFramebuffer(XOpenGL::FrameBufferType targetType, sptr<XOpenGLFramebufferObject> frameBuffer);
	static void xglBindFramebuffer(XOpenGL::FrameBufferType targetType, unsigned int frameBuffer);
	static void xglBlitFramebuffer(int srcX0, int srcY0, int srcW, int srcH, int dstX0, int dstY0, int dstW, int dstH,XOpenGL::FlagBits, XOpenGL::FilterType filterType);
	static void xglGetIntegerv(unsigned int name, int& valaue);

	//获取当前绑定的fbo
	static void xglGetBindFrameBufferId(XOpenGL::FrameBufferBindingType, int& valaue);

	static void xglPixelStorei(XOpenGL::PixelStoreParameter pname, int param);
	static void xglReadPixels(int startx,int starty, int width, int height, XOpenGL::TextureExternalFormat format, XOpenGL::DataType type, void *data);
	static void xglFlush();
	static XOpenGL::GlSyncObject xglFenceSync(XOpenGL::SyncFlags flags, XOpenGL::SyncCondition condition = XOpenGL::SyncCondition::SyncGPUCommandsComplete);
	static XOpenGL::SyncStatus xglClientWaitSync(XOpenGL::GlSyncObject sync, XOpenGL::SyncFlags flags, unsigned long long timeout/*纳秒*/);
	static void xglDeleteSync(XOpenGL::GlSyncObject sync);
};