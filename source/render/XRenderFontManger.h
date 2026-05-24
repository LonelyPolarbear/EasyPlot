#pragma once
#include "renderApi.h"

#include <dataBase/XDataObject.h>

class XOpenGLRenderWindow;
class XRenderFontManger : public XDataObject {
protected:
	XRenderFontManger(sptr<XOpenGLRenderWindow> renWindow);
	virtual ~XRenderFontManger();
public:
	/**
	 * @brief 初始化渲染底层选相关资源，一般该接口只会被调用一次
	 */
	void InitRenderResource();		
protected:
	class Internal;
	std::unique_ptr<Internal> mData;
};