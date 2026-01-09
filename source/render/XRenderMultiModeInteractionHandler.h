#pragma once
#include "XRenderApi.h"
#include "XRenderType.h"
#include "XRenderInteractionEventHandler.h"
#include <dataBase/dataobject.h>
#include <lib00_utilty/XUtilty.h>
#include <xsignal/XSignal.h>

class XRender;
class CameraNavigationHandler;
class XRenderMultiModeInteractionHandler : public XRenderInteractionEventHandler {
protected:
	XRenderMultiModeInteractionHandler();
	virtual ~XRenderMultiModeInteractionHandler();

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
public:
	void setRender(sptr< XRender> render) override;
	void setCameraNavigationHandler(sptr<CameraNavigationHandler> cameraHandler);
	sptr<CameraNavigationHandler> getCameraNavigationHandler() const;
protected:
	//包含多个模式交互处理器
	sptr<CameraNavigationHandler> mCameraHandler;
};