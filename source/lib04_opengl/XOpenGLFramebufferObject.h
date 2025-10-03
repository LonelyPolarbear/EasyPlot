#pragma once
#include "XOpenglApi.h"
#include <dataBase/dataobject.h>

#include "XOpenGLTexture.h"

class LIB04_OPENGL_API XOpenGLFramebufferObject :DataBaseObject {
public:

	enum Attachment {
		CombinedDepthStencil, 
		Depth,
		Color
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
	void bind();
	void release();
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


	bool isComplete() const;

	bool updateBufferSize(int width, int height);

private:
	class Internal;
	std::unique_ptr<Internal> d;
};