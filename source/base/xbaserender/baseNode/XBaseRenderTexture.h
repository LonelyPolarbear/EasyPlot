#pragma once
#include "base/xbaserender/xbaserenderApi.h"
#include "dataBase/XDataObject.h"
#include <lib04_opengl/XOpenGLTexture.h>

class XBaseRenderTexture : public XDataObject {
	REGISTER_CLASS_META_DATA(XBaseRenderTexture, XDataObject);
protected:
	XBaseRenderTexture();
	virtual ~XBaseRenderTexture();
public:
	sptr<XOpenGLTexture> getTexture() const { return m_texture; }
	void setTexture(sptr<XOpenGLTexture> texture) { m_texture = texture; }
protected:
	sptr<XOpenGLTexture> m_texture;
};
