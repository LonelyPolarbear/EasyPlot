#include "XOpenGLFuntion.h"
#include <glew/glew.h>
#include "XOpenGLFramebufferObject.h"
#include <regex>

void XOpenGLFuntion::xglBindFramebuffer(XOpenGL::FrameBufferType targetType, sptr<XOpenGLFramebufferObject> frameBuffer)
{
	glBindFramebuffer((unsigned int)(targetType), frameBuffer->getId());
}

void XOpenGLFuntion::xglBindFramebuffer(XOpenGL::FrameBufferType targetType, unsigned int frameBuffer)
{
	glBindFramebuffer((unsigned int)(targetType), frameBuffer);
	checkGLError();
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

void XOpenGLFuntion::xglGetBindDataBufferId(XOpenGL::DataBufferBindingType bufferType, int& bufferId)
{
	glGetIntegerv((unsigned int)(bufferType), &bufferId);
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

void XOpenGLFuntion::xglGetMaxShaderComponentTypeNum(XOpenGL::ShaderComponentType type, int& value)
{
	glGetIntegerv((unsigned int)(type), &value);
	checkGLError();
}

int XOpenGLFuntion::xglGetMaxShaderComponentTypeNum(XOpenGL::ShaderComponentType type)
{
	int value =0;
	xglGetMaxShaderComponentTypeNum(type,value);
	return value;
}

int XOpenGLFuntion::xglGetTextureUnitBindTexture(int textureUint, XOpenGL::TextureBindingType type)
{
	int textureId =0;
	glGetIntegeri_v((unsigned int)(type), textureUint, &textureId);
	return textureId;
}

int XOpenGLFuntion::xglGetActiveTexture()
{
	GLint activeTextureEnum;
	// 查询当前激活的纹理单元枚举值（如GL_TEXTURE0、GL_TEXTURE1等）
	glGetIntegerv(GL_ACTIVE_TEXTURE, &activeTextureEnum);

	return activeTextureEnum - GL_TEXTURE0;
}

std::string XOpenGLFuntion::glVersion()
{
	auto p=(char*)glGetString(GL_VERSION);
	std::string version(p);

	std::regex pattern( R"(^(\d+)\.(\d+))");
	std::smatch matches;

	// 执行匹配
	if (std::regex_search(version, matches, pattern)) {
		// matches[0] 是整个匹配的字符串（如 "4.6"）
		// matches[1] 是第一个捕获组（主版本号 "4"）
		// matches[2] 是第二个捕获组（次版本号 "6"）
		if (matches.size() >= 3) {  // 确保捕获到两个组
			int major = std::stoi(matches[1].str());  // 转换为主版本号（整数）
			int minor = std::stoi(matches[2].str());  // 转换为次版本号（整数
			return std::string(std::to_string(major) + "." + std::to_string(minor));
		}
	}

	return "";
}

bool XOpenGLFuntion::checkGLError()
{
	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		return false;
	}
	return true;
}
