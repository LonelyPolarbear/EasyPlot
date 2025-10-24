#include "XOpenGLFuntion.h"
#include <glew/glew.h>
#include "XOpenGLFramebufferObject.h"

void XOpenGLFuntion::xglBindFramebuffer(XOpenGL::FrameBufferType targetType, sptr<XOpenGLFramebufferObject> frameBuffer)
{
	glBindFramebuffer((unsigned int)(targetType), frameBuffer->getId());
}

void XOpenGLFuntion::xglBindFramebuffer(XOpenGL::FrameBufferType targetType, unsigned int frameBuffer)
{
	glBindFramebuffer((unsigned int)(targetType), frameBuffer);
}

void XOpenGLFuntion::xglBlitFramebuffer(int srcX0, int srcY0, int srcW, int srcH, int dstX0, int dstY0, int dstW, int dstH, XOpenGL::FlagBits bit, XOpenGL::FilterType filterType)
{
	glBlitFramebuffer(srcX0,srcY0,srcW,srcH,dstX0,dstY0,dstW,dstH,(unsigned int)(bit), (unsigned int)(filterType));
}

void XOpenGLFuntion::xglGetIntegerv(unsigned int name, int& valaue)
{
	glGetIntegerv(name, &valaue);
}


void XOpenGLFuntion::xglGetBindFrameBufferId(XOpenGL::FrameBufferBindingType bindingType, int& valaue)
{
	glGetIntegerv( (unsigned int)(bindingType), &valaue);
}

void XOpenGLFuntion::xglPixelStorei(XOpenGL::PixelStoreParameter pname, int param)
{
	glPixelStorei( (unsigned int)(pname), param);
}

void XOpenGLFuntion::xglReadPixels(int startx, int starty, int width, int height, XOpenGL::TextureExternalFormat format, XOpenGL::DataType type, void* data)
{
	glReadPixels(startx, starty, width, height, (unsigned int)(format), (unsigned int)(type), data);
}

void XOpenGLFuntion::xglFlush()
{
	glFlush();
}

XOpenGL::GlSyncObject XOpenGLFuntion::xglFenceSync(XOpenGL::SyncFlags flags, XOpenGL::SyncCondition condition)
{
	auto sync = glFenceSync( (unsigned int)(condition), (unsigned int)(flags) );
	XOpenGL::GlSyncObject ret;
	ret.ptr = sync;
	return ret;
}

XOpenGL::SyncStatus XOpenGLFuntion::xglClientWaitSync(XOpenGL::GlSyncObject sync, XOpenGL::SyncFlags flags, unsigned long long timeout)
{
	GLsync glSync = (GLsync)sync.ptr;
	auto ret = glClientWaitSync(glSync, (unsigned int)(flags), timeout);
	return (XOpenGL::SyncStatus)ret;
}

void XOpenGLFuntion::xglDeleteSync(XOpenGL::GlSyncObject sync)
{
	glDeleteSync((GLsync)sync.ptr);
}
