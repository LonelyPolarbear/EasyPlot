#pragma once
#include "XRenderApi.h"
#include "XRenderType.h"
#include <dataBase/XDataBaseObject.h>
#include <dataBase/XVector.h>
#include <lib00_utilty/XUtilty.h>
#include <xsignal/XSignal.h>

enum class MouseType {
	none,
	left,
	right,
	middle
};

class XInteractionEventHandler : public XDataBaseObject{
protected:
	XInteractionEventHandler();
	virtual ~XInteractionEventHandler();
public:
	void Init() override; 
	virtual void LeftButtonPressEvent(XQ::Vec2i, XQ::KeyboardModifier);
	virtual void LeftButtonReleaseEvent(XQ::Vec2i, XQ::KeyboardModifier);
	virtual void MiddleButtonPressEvent(XQ::Vec2i, XQ::KeyboardModifier);
	virtual void MiddleButtonReleaseEvent(XQ::Vec2i, XQ::KeyboardModifier);
	virtual void RightButtonPressEvent(XQ::Vec2i, XQ::KeyboardModifier);
	virtual void RightButtonReleaseEvent(XQ::Vec2i, XQ::KeyboardModifier);
	virtual void EnterEvent();
	virtual void LeaveEvent();
	virtual void FoucsInEvent();
	virtual void FoucsOutEvent();
	virtual void ResizeEvent(XQ::Vec2i);
	virtual void KeyPressEvent(XQ::Key, XQ::KeyboardModifier);
	virtual void KeyReleaseEvent(XQ::Key, XQ::KeyboardModifier);
	virtual void MouseMoveEvent(XQ::Vec2i, XQ::KeyboardModifier);
	virtual void MouseWheelForwardEvent(XQ::Vec2i, XQ::KeyboardModifier);
	virtual void MouseWheelBackwardEvent(XQ::Vec2i, XQ::KeyboardModifier);

	virtual void TimeEvent(int id,double time);

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

	XSIGNAL(void(XQ::Key, XQ::KeyboardModifier)) SigKeyPress;
	XSIGNAL(void(XQ::Key, XQ::KeyboardModifier)) SigKeyRelease;

	XSIGNAL(void(XQ::Vec2i, XQ::KeyboardModifier)) SigMouseMove;												//鼠标移动
	XSIGNAL(void(XQ::Vec2i, XQ::KeyboardModifier)) SigMouseWheelForward;								//滚轮前进
	XSIGNAL(void(XQ::Vec2i, XQ::KeyboardModifier)) SigMouseWheelBackward;								//滚轮后退
	XSIGNAL(void(int/*id*/, double/*ms*/)) SigTimeOut;
};