#pragma once
#include "renderApi.h"

#include <base/xbaserender/baseRender/XBaseRenderFontManger.h>
#include <dataBase/XDataObject.h>
#include "lib04_opengl/XOpenGLTexture.h"

class XRenderFontManger : public XBaseRenderFontManger {
protected:
	XRenderFontManger(sptr<XBaseRenderWindow> renWindow);
	virtual ~XRenderFontManger();
public:
	/**
	 * @brief 初始化渲染底层选相关资源，一般该接口只会被调用一次
	 */
	void InitRenderResource() override;		
	sptr<XDataBaseObject> getFontTexture() const override;
protected:
	class Internal;
	std::unique_ptr<Internal> mData;
};