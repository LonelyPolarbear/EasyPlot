#pragma once
#include "renderApi.h"
#include "base/xbaserender/baseRender/XRenderType.h"
#include "base/xbaserender/baseRender/XBaseInteractionEventHandler.h"
#include <dataBase/XDataBaseObject.h>
#include <dataBase/XVector.h>
#include <lib00_utilty/XUtilty.h>
#include <xsignal/XSignal.h>


class XInteractionEventHandler : public XBaseInteractionEventHandler {
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
protected:
	class Internal;
	std::unique_ptr<Internal> mData;
};