#pragma once
#include <dataBase/XDataObject.h>
#include <dataBase/XDataBaseObject.h>
#include <base/xbaserender/baseRender/XBaseRender.h>
#include <base/xbaserender/xbaserenderApi.h>
/**
 * @ class XBaseDrawManger
 * @ brief 绘制管理类
 * @ details
 * 
 * @ note
 * @ warning
 */
class XBASERENDER_API XBaseDrawManger :public XDataObject {
	REGISTER_CLASS_META_DATA(XBaseDrawManger, XDataObject);
protected:
	XBaseDrawManger();
	~XBaseDrawManger();
public:
	virtual sptr<XDataBaseObject> getScreenFbo() =0;
	virtual sptr<XDataBaseObject> getBiltFbo() =0;
	virtual sptr<XDataBaseObject> getOverlayFbo(int lay) = 0;

	virtual void InitRenderSource() =0;
	virtual void setRender(sptr<XBaseRender> render)=0;
	virtual void bilt() =0;
	virtual void SlotRenderSizeChanged(XQ::Vec2i) =0;
};