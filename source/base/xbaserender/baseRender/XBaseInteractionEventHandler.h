#pragma once
#include "base/xbaserender/xbaserenderApi.h"
#include "base/xbaserender/baseRender/XRenderType.h"
#include <dataBase/XDataObject.h>

enum class MouseType {
	none,
	left,
	right,
	middle
};

class XEvent {
public:
	void stopPropagate() {
		mIsStopPropagate = true;		//不再传播
	}

	bool isStopPropagate() {			//继续传播
		return mIsStopPropagate;
	}
protected:
	bool mIsStopPropagate = false;
};

class XBASERENDER_API XBaseInteractionEventHandler : public XDataObject {
	REGISTER_CLASS_META_DATA(XBaseInteractionEventHandler, XDataObject);
protected:
	XBaseInteractionEventHandler();
	virtual ~XBaseInteractionEventHandler();

public:
	virtual void LeftButtonPressEvent(XQ::Vec2i, XQ::KeyboardModifier, XEvent& event)=0;
	virtual void LeftButtonReleaseEvent(XQ::Vec2i, XQ::KeyboardModifier, XEvent& event)=0;
	virtual void MiddleButtonPressEvent(XQ::Vec2i, XQ::KeyboardModifier, XEvent& event)=0;
	virtual void MiddleButtonReleaseEvent(XQ::Vec2i, XQ::KeyboardModifier, XEvent& event)=0;
	virtual void RightButtonPressEvent(XQ::Vec2i, XQ::KeyboardModifier, XEvent& event)=0;
	virtual void RightButtonReleaseEvent(XQ::Vec2i, XQ::KeyboardModifier, XEvent& event)=0;
	virtual void EnterEvent(XEvent& event)=0;
	virtual void LeaveEvent(XEvent& event)=0;
	virtual void FoucsInEvent(XEvent& event)=0;
	virtual void FoucsOutEvent(XEvent& event)=0;
	virtual void ResizeEvent(XQ::Vec2i, XEvent& event) = 0;
	virtual void KeyPressEvent(XQ::Key, XQ::KeyboardModifier, XEvent& event) = 0;
	virtual void KeyReleaseEvent(XQ::Key, XQ::KeyboardModifier, XEvent& event) = 0;
	virtual void MouseMoveEvent(XQ::Vec2i, XQ::KeyboardModifier, XEvent& event) = 0;
	virtual void MouseWheelForwardEvent(XQ::Vec2i, XQ::KeyboardModifier, XEvent& event) = 0;
	virtual void MouseWheelBackwardEvent(XQ::Vec2i, XQ::KeyboardModifier, XEvent& event) = 0;
	virtual void TimeEvent(int id, double time, XEvent& event) = 0;

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