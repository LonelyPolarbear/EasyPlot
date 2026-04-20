#pragma once
#include "XOpenglApi.h"
#include <dataBase/XDataBaseObject.h>
#include "XOpenGLType.h"
#include "XOpenGLTexture.h"

class LIB04_OPENGL_API XOpenGLFramebufferObject :XDataBaseObject {
public:
	enum Attachment {
		CombinedDepthStencil, 
		Depth,				//深度附件
		Color,				//颜色附件
		stencil,				//模板附件
	};
	enum
		FramebufferRestorePolicy {
		DontRestoreFramebufferBinding, 
		RestoreFramebufferBindingToDefault, 
		RestoreFrameBufferBinding
	};

protected:
	XOpenGLFramebufferObject(int width, int height);
	XOpenGLFramebufferObject();
	~XOpenGLFramebufferObject();
public:
	bool create();
	void bind(XOpenGL::FrameBufferType type = XOpenGL::FrameBufferType::framebuffer);
	void release(XOpenGL::FrameBufferType type = XOpenGL::FrameBufferType::framebuffer);
	void destory();
	uint32_t getId() const;

	void setWidth(int width);
	void setHeight(int height);
	int getWidth() const;
	int getHeight() const;

	std::shared_ptr<XOpenGLTexture>getColorAttachment(int index = 0) const;

	std::shared_ptr<XOpenGLTexture>getDepthAttachment() const;

	/**
	 * @brief 绑定纹理附件，外部指定纹理参数，纹理在函数内部创建
	 * @parm[in] attachment 纹理附件类型
	 * @parm[in] internalFormat 纹理内部格式
	 * @parm[in] inputdataPixelFormat 外部数据格式
	 * @parm[in] inputdataPixelType 外部数据类型
	 * @parm[in] index 如果是颜色附件，表示附加到第几个颜色附件（GL_COLOR_ATTACHMENT0+index）
	 */
	void addAttachment(Attachment attachment, 
				XOpenGLTexture::TextureFormat internalFormat, 
				XOpenGLTexture::PixelFormat inputdataPixelFormat, 
				XOpenGLTexture::PixelType inputdataPixelType,
				int index =0
				);

	/**
	 * @brief 绑定纹理附件，该附件由外部提供
	 * @parm[in]  attachment 纹理附件类型
	 * @parm[in]  texture 外部提供的纹理对象
	 * @parm[in]  index 如果是颜色附件，表示附加到第几个颜色附件（GL_COLOR_ATTACHMENT0+index）
	 * @parm[in] level Mipmap级别，通常为0
	 * @parm[in] layer 3D纹理 表示深度切片，或者2d纹理数组的某一层
	 */
	void addAttachment(Attachment attachment,sptr<XOpenGLTexture> texture,int index = 0,int level=0,int layer =0);

	/**
	 * @brief 绑定多重采样纹理附件，外部指定纹理参数，纹理在函数内部创建
	 * @parm[in] attachment 纹理附件类型
	 * @parm[in] internalFormat 纹理内部格式
	 * @parm[in] inputdataPixelFormat 外部数据格式
	 * @parm[in] inputdataPixelType 外部数据类型
	 * @parm[in] index 如果是颜色附件，表示附加到第几个颜色附件（GL_COLOR_ATTACHMENT0+index）
	 */
	void addAttachmentMSAA(Attachment attachment,
		XOpenGLTexture::TextureFormat internalFormat,
		XOpenGLTexture::PixelFormat inputdataPixelFormat,
		XOpenGLTexture::PixelType inputdataPixelType,
		int index = 0
	);

	/**
	 * @brief 判断fbo完整性
	 */
	bool isComplete() const;

	/**
	 * @brief fbo尺寸更改，内部需要重新更新纹理尺寸，并重新绑定纹理附件，默认操作的是二维纹理附件
	 */
	bool updateBufferSize(int width, int height);

	XOpenGL::FrameBufferBindingType getBindingType(XOpenGL::FrameBufferType type) const;
private:
	class Internal;
	std::unique_ptr<Internal> d;
};