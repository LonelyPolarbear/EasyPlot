#include "XBaseRenderTexture.h"

XBaseRenderTexture::XBaseRenderTexture()
{
	m_texture = makeShareDbObject<XOpenGLTexture>();
}

XBaseRenderTexture::~XBaseRenderTexture()
{

}
