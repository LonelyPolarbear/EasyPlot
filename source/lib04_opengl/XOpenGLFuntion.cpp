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

std::optional<XQ::Vec2i> XOpenGLFuntion::glVersion2()
{
	auto p = (char*)glGetString(GL_VERSION);

	checkGLError();
	std::string version(p);

	std::regex pattern(R"(^(\d+)\.(\d+))");
	std::smatch matches;

	// 执行匹配
	if (std::regex_search(version, matches, pattern)) {
		// matches[0] 是整个匹配的字符串（如 "4.6"）
		// matches[1] 是第一个捕获组（主版本号 "4"）
		// matches[2] 是第二个捕获组（次版本号 "6"）
		if (matches.size() >= 3) {  // 确保捕获到两个组
			int major = std::stoi(matches[1].str());  // 转换为主版本号（整数）
			int minor = std::stoi(matches[2].str());  // 转换为次版本号（整数
			return XQ::Vec2i(major, minor);
		}
	}
	return std::nullopt;
}

std::optional<int> XOpenGLFuntion::glVersion3()
{
	if (auto version = glVersion2()) {
		return (*version)[0] * 10 + (*version)[1];
	}
	return std::nullopt;
}

bool XOpenGLFuntion::isSupport4_5()
{
	if (auto v = glVersion3()) {
		return v >=45;
	}
	return false;
}

bool XOpenGLFuntion::isSupport4_2()
{
	if (auto v = glVersion3()) {
		return v >= 42;
	}
	return false;
}

bool XOpenGLFuntion::isSupport3_3()
{
	if (auto v = glVersion3()) {
		return v >= 33;
	}
	return false;
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

void XOpenGLFuntion::xglDepthFunc(XOpenGL::DepthOrStencilCompFunType fun)
{
	glDepthFunc((GLenum)fun);
	checkGLError();
}

void XOpenGLFuntion::xglStencilFunc(XOpenGL::DepthOrStencilCompFunType fun, int ref, uint32_t mask)
{
	glStencilFunc((GLenum)fun,ref,mask);
}

void XOpenGLFuntion::xGetStencilFunc(XOpenGL::DepthOrStencilCompFunType& fun, int& ref, uint32_t& mask)
{
	GLint func_, ref_,mask_;
	glGetIntegerv(GL_STENCIL_FUNC, &func_);
	glGetIntegerv(GL_STENCIL_REF, &ref_);
	glGetIntegerv(GL_STENCIL_VALUE_MASK, &mask_);
	checkGLError();
	fun = (XOpenGL::DepthOrStencilCompFunType)func_;
	ref = ref_;
	mask =mask_;
}

void XOpenGLFuntion::xglStencilOp(XOpenGL::StencilBehavior sfail, XOpenGL::StencilBehavior zfail, XOpenGL::StencilBehavior zpass)
{
	glStencilOp((GLenum)sfail, (GLenum)zfail, (GLenum)zpass);
	checkGLError();
}

void XOpenGLFuntion::xGetStencilOp(XOpenGL::StencilBehavior& sfail, XOpenGL::StencilBehavior& zfail, XOpenGL::StencilBehavior& zpass)
{
	GLint sfail_, zfail_, zpass_;
	glGetIntegerv(GL_STENCIL_FAIL, &sfail_);
	glGetIntegerv(GL_STENCIL_PASS_DEPTH_FAIL, &zfail_);
	glGetIntegerv(GL_STENCIL_PASS_DEPTH_PASS, &zpass_);
	checkGLError();
	sfail = (XOpenGL::StencilBehavior)sfail_;
	zfail = (XOpenGL::StencilBehavior)zfail_;
	zpass = (XOpenGL::StencilBehavior)zpass_;
}

void XOpenGLFuntion::xglBlendFunc(XOpenGL::BlendFuncFactor srcFactor, XOpenGL::BlendFuncFactor dstFactor)
{
	glBlendFunc((GLenum)srcFactor,(GLenum)dstFactor);
	checkGLError();
}

void XOpenGLFuntion::xGetBlendFunc(XOpenGL::BlendFuncFactor& srcFactor, XOpenGL::BlendFuncFactor& dstFactor)
{
	GLint srcFactor_, dstFactor_;

	glGetIntegerv(GL_BLEND_SRC_RGB, &srcFactor_);
	glGetIntegerv(GL_BLEND_DST_RGB, &dstFactor_);

	srcFactor = (XOpenGL::BlendFuncFactor)	srcFactor_;
	dstFactor = (XOpenGL::BlendFuncFactor)	dstFactor_;
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

XQ::Recti XOpenGLFuntion::xglViewport(XQ::Recti rect)
{
	//读取之前的视口参数
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glViewport(rect[0],rect[1],rect[2],rect[3]);
	checkGLError();
	return XQ::Recti(viewport[0],viewport[1],viewport[2],viewport[3]);
}

XQ::Recti XOpenGLFuntion::xglglScissor(XQ::Recti rect)
{
	//读取之前的视口参数
	GLint scissor[4];
	glGetIntegerv(GL_SCISSOR_BOX, scissor);
	glScissor(rect[0], rect[1], rect[2], rect[3]);
	checkGLError();
	return XQ::Recti(scissor[0],scissor[1],scissor[2],scissor[3]);
}

std::optional<XQ::Vec3i> XOpenGLFuntion::xGetTextureSize(int textureID, XOpenGL::TextureTarget target)
{
	XQ::Vec3i size;
	if (auto version =glVersion3()) {
		if (version >= 45) {
			glGetTextureLevelParameteriv(textureID, 0, GL_TEXTURE_WIDTH, &size[0]);
			glGetTextureLevelParameteriv(textureID, 0, GL_TEXTURE_HEIGHT, &size[1]);
			glGetTextureLevelParameteriv(textureID, 0, GL_TEXTURE_DEPTH, &size[2]);
			return size;
		}
		else {
			glBindTexture((GLenum)target, textureID);
			glGetTexLevelParameteriv((GLenum)target, 0, GL_TEXTURE_WIDTH, &size[0]);
			glGetTexLevelParameteriv((GLenum)target, 0, GL_TEXTURE_HEIGHT, &size[1]);
			glGetTexLevelParameteriv((GLenum)target, 0, GL_TEXTURE_DEPTH, &size[2]);
			//glBindTexture((GLenum)target, 0); // 恢复状态
			return size;
		}
	}
	return std::nullopt;
}

std::optional<int> XOpenGLFuntion::xGetTextureSampleNum(int textureId, XOpenGL::TextureTarget target)
{
	int samples =0;
	if (auto version = glVersion3()) {
		if (version >= 45) {
			glGetTextureLevelParameteriv(textureId, 0, GL_TEXTURE_SAMPLES, &samples);
		}
		else {
			glBindTexture((GLenum)target, textureId);
			GLint samples = 0;
			glGetTexLevelParameteriv((GLenum)target, 0, GL_TEXTURE_SAMPLES, &samples);
			glBindTexture((GLenum)target, 0); // 可选解绑
		}
	}
	return std::nullopt;
}
//glGetTextureLevelParameteriv(textureID, 0, GL_TEXTURE_SAMPLES, &samples);
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