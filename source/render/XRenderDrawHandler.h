#pragma once
#include "renderApi.h"
#include "base/xbaserender/baseRender/XRenderType.h"
#include "XRenderInteractionEventHandler.h"
#include <dataBase/XDataBaseObject.h>
#include <dataBase/XVector.h>
#include <lib00_utilty/XUtilty.h>
#include <xsignal/XSignal.h>
#include <xrendernode/XRenderNode.h>
class XRender;

class XRenderDrawHandler : public XRenderInteractionEventHandler {
protected:
	XRenderDrawHandler();
	virtual ~XRenderDrawHandler();
public:
	virtual void LeftButtonPressEvent(XQ::Vec2i, XQ::KeyboardModifier, XEvent& event);
	virtual void LeftButtonReleaseEvent(XQ::Vec2i, XQ::KeyboardModifier, XEvent& event);
	virtual void MiddleButtonPressEvent(XQ::Vec2i, XQ::KeyboardModifier, XEvent& event);
	virtual void MiddleButtonReleaseEvent(XQ::Vec2i, XQ::KeyboardModifier, XEvent& event);
	virtual void RightButtonPressEvent(XQ::Vec2i, XQ::KeyboardModifier, XEvent& event);
	virtual void RightButtonReleaseEvent(XQ::Vec2i, XQ::KeyboardModifier, XEvent& event);
	virtual void LeftButtonDoublePressEvent(XQ::Vec2i, XQ::KeyboardModifier, XEvent& event);
	virtual void RightButtonDoublePressEvent(XQ::Vec2i, XQ::KeyboardModifier, XEvent& event);
	virtual void MiddleButtonDoublePressEvent(XQ::Vec2i, XQ::KeyboardModifier, XEvent& event);
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
	bool isRenderActive() const;

	void setRender(sptr< XRender> render) override;

	void setExtInterfaceEbable(bool enable);

	XSIGNAL(void(bool) )SigDrawModeChanged;
protected:
	void slotRenderActiveChanged(bool active) override;
	
protected:
	struct Internal;
	sptr< Internal> mData;
};