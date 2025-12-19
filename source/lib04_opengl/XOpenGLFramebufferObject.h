#pragma once
#include "XOpenglApi.h"
#include <dataBase/dataobject.h>
#include "XOpenGLType.h"
#include "XOpenGLTexture.h"

class LIB04_OPENGL_API XOpenGLFramebufferObject :DataBaseObject {
public:
	/*enum AttachmentDSMode {
		DepthStencil,
		Depth,
		DepthAndStencil,
	};*/
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

	void addAttachment(Attachment attachment, 
				XOpenGLTexture::TextureFormat internalFormat, 
				XOpenGLTexture::PixelFormat inputdataPixelFormat, 
				XOpenGLTexture::PixelType inputdataPixelType,
				int index =0
				);

	void addAttachment(Attachment attachment,sptr<XOpenGLTexture> texture,int index = 0);

	void addAttachmentMSAA(Attachment attachment,
		XOpenGLTexture::TextureFormat internalFormat,
		XOpenGLTexture::PixelFormat inputdataPixelFormat,
		XOpenGLTexture::PixelType inputdataPixelType,
		int index = 0
	);


	bool isComplete() const;

	bool updateBufferSize(int width, int height);

	XOpenGL::FrameBufferBindingType getBindingType(XOpenGL::FrameBufferType type) const;
private:
	class Internal;
	std::unique_ptr<Internal> d;
};