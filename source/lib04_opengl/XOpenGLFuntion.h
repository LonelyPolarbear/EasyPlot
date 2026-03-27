#pragma once
#include "XOpenglApi.h"
#include "XOpenGLType.h"
#include "dataBase/XDataBaseObject.h"
#include "dataBase/XVector.h"

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

	static void xglGetBindDataBufferId(XOpenGL::DataBufferBindingType bufferType, int& bufferId);

	static int xglPixelStorei(XOpenGL::PixelStoreParameter pname, int param);

	static void xglReadPixels(int startx,int starty, int width, int height, XOpenGL::TextureExternalFormat format, XOpenGL::DataType type, void *data);

	static void xglFlush();

	static XOpenGL::GlSyncObject xglFenceSync(XOpenGL::SyncFlags flags, XOpenGL::SyncCondition condition = XOpenGL::SyncCondition::SyncGPUCommandsComplete);

	static XOpenGL::SyncStatus xglClientWaitSync(XOpenGL::GlSyncObject sync, XOpenGL::SyncFlags flags, unsigned long long timeout/*纳秒*/);

	static void xglDeleteSync(XOpenGL::GlSyncObject sync);

	static void xglGetMaxShaderComponentTypeNum(XOpenGL::ShaderComponentType,int &value);

	static int xglGetMaxShaderComponentTypeNum(XOpenGL::ShaderComponentType);

	//获取指定的纹理单元绑定的纹理对象的id
	static int xglGetTextureUnitBindTexture(int textureUint,XOpenGL::TextureBindingType type);

	/// <summary>
	/// 获取当前激活的纹理单元
	/// </summary>
	static int xglGetActiveTexture();

	/**
	*@brief 获取当前使用的opengl版本号，返回格式为"major.minor
	*/
	static std::string glVersion();

	/**
	*@brief 获取当前使用的opengl版本号
	*/
	static std::optional<XQ::Vec2i> glVersion2();

	/**
	*@brief 获取当前使用的opengl版本号，返回数值major*10+minor
	*/
	static std::optional<int> glVersion3();

	/**
	 * @brief 检测是否支持OpenGL 4.5
	 */
	static bool isSupport4_5();

	/**
	 * @brief 检测是否支持OpenGL 4.2
	 */
	static bool isSupport4_2();

	/**
	 * @brief 检测是否支持OpenGL 3.3
	 */
	static bool isSupport3_3();

	/**
	 * @brief 检测opengl接口调用是否出错，true表示没有错误，false表示有错误
	 */
	static bool checkGLError();

	/// <summary>
	///  控制颜色缓冲区各个颜色分量的写入
	/// </summary>
	/// <param name="red">red通道是否允许写入，true表示允许写入，false表示禁止写入</param>
	/// <param name="green">green通道是否允许写入，true表示允许写入，false表示禁止写入</param>
	/// <param name="blue">blue通道是否允许写入，true表示允许写入，false表示禁止写入</param>
	/// <param name="alpha">alpha通道是否允许写入，true表示允许写入，false表示禁止写入</param>
	static void xglColorMask(bool red, bool green, bool blue, bool alpha);

	/// <summary>
	/// 控制深度缓冲区的写入，当禁用深度写入时，深度缓冲区不会更新，但深度测试仍然有效（如果启用的了的话）
	/// </summary>
	/// <param name="flag">true表示允许写入，false表示禁止写入</param>
	static void xglDepthMask(bool flag);

	/// <summary>
	/// 控制模板值写入时的位掩码 位1 表示该位置允许写入，位0表示禁止写入
	/// </summary>
	static void xglStencilMask(unsigned int mask);

	/// <summary>
	/// 正面的模板写入掩码
	/// </summary>
	static void xglStencilMaskFront(unsigned int mask);

	/// <summary>
	/// 背面的模板写入掩码
	/// </summary>
	static void xglStencilMaskBack(unsigned int mask);

	/// <summary>
	/// 清除当前绑定的FBO的缓冲区
	/// </summary>
	/// <param name="bits"> 要清除的缓冲区类型，可以取或</param>
	static void xglClear(/*XOpenGL::BufferBits*/ unsigned int bits);

	/// <summary>
	/// 设置当前绑定的FBO的颜色缓冲清除值
	/// </summary>
	static void xglClearColor(float red, float green, float blue, float alpha);

	/// <summary>
	/// 设置当前绑定的FBO的深度缓冲清除值[0-1],超出范围的值会被钳制到 [0.0, 1.0]
	/// </summary>
	static void xglClearDepth(float depth);

	/// <summary>
	/// 设置当前绑定的FBO的模板缓冲清除值[0-255]
	/// </summary>
	static void xglClearStencil(int s);

	static std::vector<std::string> xglGetExtensions();

	static bool xisExtensionSupported(const std::string& extensionName);

	static XQ::Recti xglViewport(XQ::Recti rect);

	static XQ::Recti xglglScissor(XQ::Recti rect);

	static std::optional<XQ::Vec3i> xGetTextureSize(int textureID, XOpenGL::TextureTarget target);

	static std::optional<int> xGetTextureSampleNum(int textureId, XOpenGL::TextureTarget target);

};