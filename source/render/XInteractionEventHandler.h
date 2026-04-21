#pragma once
#include "renderApi.h"
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

class XInteractionEventHandler : public XDataBaseObject{
protected:
	XInteractionEventHandler();
	virtual ~XInteractionEventHandler();
public:
	void Init() override; 
	virtual void LeftButtonPressEvent(XQ::Vec2i, XQ::KeyboardModifier, XEvent &event);
	virtual void LeftButtonReleaseEvent(XQ::Vec2i, XQ::KeyboardModifier, XEvent& event);
	virtual void MiddleButtonPressEvent(XQ::Vec2i, XQ::KeyboardModifier, XEvent& event);
	virtual void MiddleButtonReleaseEvent(XQ::Vec2i, XQ::KeyboardModifier, XEvent& event);
	virtual void RightButtonPressEvent(XQ::Vec2i, XQ::KeyboardModifier, XEvent& event);
	virtual void RightButtonReleaseEvent(XQ::Vec2i, XQ::KeyboardModifier, XEvent& event);
	virtual void EnterEvent(XEvent& event);
	virtual void LeaveEvent(XEvent& event);
	virtual void FoucsInEvent(XEvent& event);
	virtual void FoucsOutEvent(XEvent& event);
	virtual void ResizeEvent(XQ::Vec2i, XEvent& event);
	virtual void KeyPressEvent(XQ::Key, XQ::KeyboardModifier, XEvent& event);
	virtual void KeyReleaseEvent(XQ::Key, XQ::KeyboardModifier, XEvent& event);
	virtual void MouseMoveEvent(XQ::Vec2i, XQ::KeyboardModifier, XEvent& event);
	virtual void MouseWheelForwardEvent(XQ::Vec2i, XQ::KeyboardModifier, XEvent& event);
	virtual void MouseWheelBackwardEvent(XQ::Vec2i, XQ::KeyboardModifier, XEvent& event);

	virtual void TimeEvent(int id,double time, XEvent& event);

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
protected:
	class Internal;
	std::unique_ptr<Internal> mData;
};