#include "XOpenGLFuntion.h"
#include <glew/glew.h>
#include "XOpenGLFramebufferObject.h"
#include <regex>

void XOpenGLFuntion::xglBindFramebuffer(XOpenGL::FrameBufferType targetType, sptr<XOpenGLFramebufferObject> frameBuffer)
{
	glBindFramebuffer((unsigned int)(targetType), frameBuffer->getId());
	checkGLError();
}

void XOpenGLFuntion::xglBindFramebuffer(XOpenGL::FrameBufferType targetType, unsigned int frameBuffer)
{
	glBindFramebuffer((unsigned int)(targetType), frameBuffer);
	checkGLError();
}

void XOpenGLFuntion::xglBlitFramebuffer(int srcX0, int srcY0, int srcW, int srcH, int dstX0, int dstY0, int dstW, int dstH, XOpenGL::FlagBits bit, XOpenGL::FilterType filterType)
{
	glBlitFramebuffer(srcX0,srcY0,srcW,srcH,dstX0,dstY0,dstW,dstH,(unsigned int)(bit), (unsigned int)(filterType));
	checkGLError();
}

void XOpenGLFuntion::xglGetIntegerv(unsigned int name, int& valaue)
{
	glGetIntegerv(name, &valaue);
	checkGLError();
}


void XOpenGLFuntion::xglGetBindFrameBufferId(XOpenGL::FrameBufferBindingType bindingType, int& valaue)
{
	glGetIntegerv( (unsigned int)(bindingType), &valaue);
	checkGLError();
}

void XOpenGLFuntion::xglGetBindDataBufferId(XOpenGL::DataBufferBindingType bufferType, int& bufferId)
{
	glGetIntegerv((unsigned int)(bufferType), &bufferId);
	checkGLError();
}

int XOpenGLFuntion::xglPixelStorei(XOpenGL::PixelStoreParameter pname, int param)
{
	int oldConfig = 0;
	glGetIntegerv((unsigned int)pname, &oldConfig);

	glPixelStorei( (unsigned int)(pname), param);
	checkGLError();
	return oldConfig;
}

void XOpenGLFuntion::xglReadPixels(int startx, int starty, int width, int height, XOpenGL::TextureExternalFormat format, XOpenGL::DataType type, void* data)
{
	glReadPixels(startx, starty, width, height, (unsigned int)(format), (unsigned int)(type), data);
	checkGLError();
}

void XOpenGLFuntion::xglFlush()
{
	glFlush();
	checkGLError();
}

XOpenGL::GlSyncObject XOpenGLFuntion::xglFenceSync(XOpenGL::SyncFlags flags, XOpenGL::SyncCondition condition)
{
	auto sync = glFenceSync( (unsigned int)(condition), (unsigned int)(flags) );
	XOpenGL::GlSyncObject ret;
	ret.ptr = sync;
	checkGLError();
	return ret;
}

XOpenGL::SyncStatus XOpenGLFuntion::xglClientWaitSync(XOpenGL::GlSyncObject sync, XOpenGL::SyncFlags flags, unsigned long long timeout)
{
	GLsync glSync = (GLsync)sync.ptr;
	auto ret = glClientWaitSync(glSync, (unsigned int)(flags), timeout);
	checkGLError();
	return (XOpenGL::SyncStatus)ret;
}

void XOpenGLFuntion::xglDeleteSync(XOpenGL::GlSyncObject sync)
{
	glDeleteSync((GLsync)sync.ptr);
	checkGLError();
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
	checkGLError();
	return value;
}

int XOpenGLFuntion::xglGetTextureUnitBindTexture(int textureUint, XOpenGL::TextureBindingType type)
{
	int textureId =0;
	glGetIntegeri_v((unsigned int)(type), textureUint, &textureId);
	checkGLError();
	return textureId;
}

int XOpenGLFuntion::xglGetActiveTexture()
{
	GLint activeTextureEnum;
	// 查询当前激活的纹理单元枚举值（如GL_TEXTURE0、GL_TEXTURE1等）
	glGetIntegerv(GL_ACTIVE_TEXTURE, &activeTextureEnum);

	checkGLError();
	return activeTextureEnum - GL_TEXTURE0;
}

std::string XOpenGLFuntion::glVersion()
{
	auto p=(char*)glGetString(GL_VERSION);

	checkGLError();
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

void XOpenGLFuntion::xglColorMask(bool red, bool green, bool blue, bool alpha)
{
	glColorMask(red, green, blue, alpha);
	checkGLError();
}

void XOpenGLFuntion::xglDepthMask(bool flag)
{
	glDepthMask(flag);
	checkGLError();
}

void XOpenGLFuntion::xglStencilMask(unsigned int mask)
{
	glStencilMask(mask);
	checkGLError();
}

void XOpenGLFuntion::xglStencilMaskFront(unsigned int mask)
{
	glStencilMaskSeparate(GL_FRONT, mask);
	checkGLError();
}

void XOpenGLFuntion::xglStencilMaskBack(unsigned int mask)
{
	glStencilMaskSeparate(GL_BACK, mask);
	checkGLError();
}

void XOpenGLFuntion::xglClear(unsigned int bits)
{
	glClear(bits);
	checkGLError();
}

void XOpenGLFuntion::xglClearColor(float red, float green, float blue, float alpha)
{
	glClearColor(red, green, blue, alpha);
	checkGLError();
}

void XOpenGLFuntion::xglClearDepth(float depth)
{
	glClearDepth(depth);
	checkGLError();
}

void XOpenGLFuntion::xglClearStencil(int s)
{
	glClearStencil(s);
	checkGLError();
}

std::vector<std::string> XOpenGLFuntion::xglGetExtensions()
{
	std::vector<std::string> names;
	// 获取扩展数量
	GLint numExtensions = 0;
	glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);

	// 遍历扩展列表
	for (int i = 0; i < numExtensions; ++i) {
		const char* extension = (const char*)glGetStringi(GL_EXTENSIONS, i);
		names.push_back(std::string(extension));
	}
	return names;
}

bool XOpenGLFuntion::xisExtensionSupported(const std::string& extensionName)
{
	GLint numExtensions = 0;
	glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);

	// 遍历所有扩展
	for (GLint i = 0; i < numExtensions; i++) {
		const char* ext = (const char*)glGetStringi(GL_EXTENSIONS, i);
		if (extensionName == std::string(ext)) {
			return true;
		}
	}
	return false;
}

/*
最大本地工作组大小限制
glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &maxLocalWorkGroupSize[0]); // X维度
glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &maxLocalWorkGroupSize[1]); // Y维度
glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &maxLocalWorkGroupSize[2]); // Z维度

最大全局工作组数量限制
glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &maxComputeWorkGroupCount[0]); // X维度
glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &maxComputeWorkGroupCount[1]); // Y维度
glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &maxComputeWorkGroupCount[2]); // Z维度
*/