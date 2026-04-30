#pragma once
#include "renderApi.h"
#include "base/xbaserender/baseRender/XRenderType.h"
#include "base/xbaserender/baseRender/XBaseRenderWindowEventDispatch.h"
#include <dataBase/XDataBaseObject.h>
#include <dataBase/XVector.h>
#include <lib00_utilty/XUtilty.h>
#include <xsignal/XSignal.h>

//需要定义窗口信号
class XOpenGLRenderWindow;
class RENDER_API XRenderWindowEventDispatch :public XBaseRenderWindowEventDispatch {
protected:
	XRenderWindowEventDispatch();
	virtual ~XRenderWindowEventDispatch();
	wptr< XOpenGLRenderWindow> m_renderWindow;
public:
	void setRenderWindow(sptr<XBaseRenderWindow> ren) override;
	sptr<XBaseRenderWindow> getRenderWindow() const override;
 };