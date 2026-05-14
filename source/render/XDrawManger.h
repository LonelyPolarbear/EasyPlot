#pragma once
#include <dataBase/XDataObject.h>
#include <dataBase/XDataBaseObject.h>
#include <base/xbaserender/baseRender/XBaseRender.h>
#include <base/xbaserender/baseRender/XBaseDrawManger.h>
#include "renderApi.h"
/**
 * @ class XDrawManger
 * @ brief 绘制管理类
 * @ details
 * 
 * @ note
 * @ warning
 */
class RENDER_API XDrawManger :public XBaseDrawManger {
	REGISTER_CLASS_META_DATA(XDrawManger, XBaseDrawManger);
protected:
	XDrawManger();
	~XDrawManger();
public:
	sptr<XDataBaseObject> getScreenFbo();
	sptr<XDataBaseObject> getBiltFbo();
	sptr<XDataBaseObject> getOverlayFbo(int lay);

	void InitRenderSource();
	void setRender(sptr<XBaseRender> render);
	void bilt();
	void SlotRenderSizeChanged(XQ::Vec2i);
protected:
	class Internal;
	std::unique_ptr<Internal> mData;
};