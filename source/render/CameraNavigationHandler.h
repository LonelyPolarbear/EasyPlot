#pragma once
#include "XRenderApi.h"
#include "XRenderType.h"
#include "XRenderInteractionEventHandler.h"
#include <dataBase/dataobject.h>
#include <dataBase/XVector.h>
#include <lib00_utilty/XUtilty.h>
#include <xsignal/XSignal.h>

class XRender;
class CameraNavigationHandler : public XRenderInteractionEventHandler {
protected:
	CameraNavigationHandler();
	virtual ~CameraNavigationHandler();
public:
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
	bool isRenderActive() const;

	void setRender(sptr< XRender> render) override;
protected:
	struct Internal;
	sptr< Internal> mData;
};