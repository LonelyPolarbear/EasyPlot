#pragma once
#include "renderApi.h"
#include "XRenderType.h"
#include "XRenderInteractionEventHandler.h"
#include <dataBase/XDataBaseObject.h>
#include <lib00_utilty/XUtilty.h>
#include <xsignal/XSignal.h>

class XRender;
class XRenderNode;
class CameraNavigationHandler;
class XRenderPickHandler;
class XManipulatorHandler;

class XRenderMultiModeInteractionHandler : public XRenderInteractionEventHandler {
protected:
	XRenderMultiModeInteractionHandler();
	virtual ~XRenderMultiModeInteractionHandler();

	virtual void LeftButtonPressEvent(XQ::Vec2i, XQ::KeyboardModifier, XEvent& event);
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
public:
	void setRender(sptr< XRender> render) override;
	void setCameraNavigationHandler(sptr<CameraNavigationHandler> cameraHandler);
	void setPickHandler(sptr<XRenderPickHandler> pickHandler);
	void setManipulatorHandler(sptr<XManipulatorHandler> manipulatorHandler);
	sptr<CameraNavigationHandler> getCameraNavigationHandler() const;
	sptr<XRenderPickHandler> getPickHandler() const;
	sptr<XManipulatorHandler> getManipulatorHandler() const;

	bool hasMode(XQ::InteractMode mode);
	void setMode(XQ::InteractMode mode);
	XQ::InteractMode getMode() const;

	/**
	 * @breif 信号：mPickHandler 选中渲染节点,此处将mPickHandler的SigRenderNodeSelected信号与其绑定，主要是解耦
	 */
	XSIGNAL(void(sptr<XRenderNode>)) SigRenderNodeSelected;
	XSIGNAL(void(XQ::InteractMode)) SigInteractModeChange;
protected:
	//包含多个模式交互处理器
	sptr<CameraNavigationHandler> mCameraHandler;					//相机
	sptr<XRenderPickHandler> mPickHandler;									//拾取
	sptr<XManipulatorHandler> mManipulatorHandler;					//操作柄
	XQ::InteractMode mCurrentMode;												//操作模式,每种模式优先级不同 TODO 待优化
	class Internal;
	std::unique_ptr<Internal> mData;
};