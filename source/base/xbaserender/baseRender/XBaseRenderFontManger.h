#pragma once
#include "base/xbaserender/xbaserenderApi.h"

#include <dataBase/XDataObject.h>

class XBaseRenderWindow;
class XBaseRenderFontManger : public XDataObject {
protected:
	XBaseRenderFontManger();
	virtual ~XBaseRenderFontManger();
public:
	/**
	 * @brief 初始化渲染底层选相关资源，一般该接口只会被调用一次
	 */
	virtual void InitRenderResource() =0;
	virtual sptr<XDataBaseObject> getFontTexture() const=0;
};