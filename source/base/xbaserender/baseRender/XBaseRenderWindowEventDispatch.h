#pragma once
#include "base/xbaserender/xbaserenderApi.h"
#include "base/xbaserender/baseRender/XRenderType.h"
#include <dataBase/XDataBaseObject.h>
#include <dataBase/XVector.h>
#include <lib00_utilty/XUtilty.h>
#include <xsignal/XSignal.h>

//需要定义窗口信号
class XBaseRenderWindow;
class XBASERENDER_API XBaseRenderWindowEventDispatch :public XDataBaseObject {
protected:
	XBaseRenderWindowEventDispatch();
	virtual ~XBaseRenderWindowEventDispatch();
public:
	virtual void setRenderWindow(sptr<XBaseRenderWindow> ren) =0;
	virtual sptr<XBaseRenderWindow> getRenderWindow() const =0;

public:
	//窗口信号
	XSIGNAL(void(XQ::Vec2i, XQ::KeyboardModifier)) SigLeftButtonPress;										//鼠标左键摁下
	XSIGNAL(void(XQ::Vec2i, XQ::KeyboardModifier)) SigLeftButtonRelease;										//鼠标左键弹起
	XSIGNAL(void(XQ::Vec2i, XQ::KeyboardModifier)) SigMiddleButtonPress;									//鼠标中键摁下
	XSIGNAL(void(XQ::Vec2i, XQ::KeyboardModifier)) SigMiddleButtonRelease;								//鼠标中键弹起
	XSIGNAL(void(XQ::Vec2i, XQ::KeyboardModifier)) SigRightButtonPress;										//鼠标右键摁下
	XSIGNAL(void(XQ::Vec2i, XQ::KeyboardModifier)) SigRightButtonRelease;									//鼠标右键弹起

	XSIGNAL(void()) SigEnter;
	XSIGNAL(void()) SigLeave;

	XSIGNAL(void()) SigFoucsIn;
	XSIGNAL(void()) SigFoucsOut;

	XSIGNAL(void(XQ::Vec2i)) SigResize;

	XSIGNAL(void(XQ::Key,XQ::KeyboardModifier)) SigKeyPress;
	XSIGNAL(void(XQ::Key, XQ::KeyboardModifier)) SigKeyRelease;

	XSIGNAL(void(XQ::Vec2i, XQ::KeyboardModifier)) SigMouseMove;												//鼠标移动
	XSIGNAL(void(XQ::Vec2i, XQ::KeyboardModifier)) SigMouseWheelForward;								//滚轮前进
	XSIGNAL(void(XQ::Vec2i, XQ::KeyboardModifier)) SigMouseWheelBackward;								//滚轮后退

	XSIGNAL(void(int/*id*/, double/*ms*/)) SigTimeOut;

	XSIGNAL(void(int id, void*/*data*/)) SigUserEvent;																		//用户自定义事件
	XSIGNAL(void(XQ::PreDefineEvent, void*/*data*/)) SigPredefineEvent;										//预定义业务逻辑事件
 };