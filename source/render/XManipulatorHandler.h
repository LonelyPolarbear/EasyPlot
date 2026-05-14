#pragma once
#include "renderApi.h"
#include "base/xbaserender/baseRender/XRenderType.h"
#include "XRenderInteractionEventHandler.h"
#include <dataBase/XDataBaseObject.h>
#include <dataBase/XVector.h>
#include <lib00_utilty/XUtilty.h>
#include <xsignal/XSignal.h>

class XRender;
class XBaseRenderNode;
class XManipulatorHandler : public XRenderInteractionEventHandler {
protected:
	XManipulatorHandler();
	virtual ~XManipulatorHandler();
public:
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
	bool isRenderActive() const;

	void setRender(sptr< XRender> render) override;
	XSIGNAL(void(sptr<XBaseRenderNode>,XQ::Vec4i)) SigRenderNodeHasSelected;				//[in]
	XSIGNAL(void(bool)) SigManipulatorModeChanged;										//[in] true表示进入编辑模式 false表示离开编辑模式
protected:
	void slotRenderActiveChanged(bool active) override;
	void slotRenderNodeHasSelected(sptr<XBaseRenderNode> node, XQ::Vec4i obj_data);
	void slotManipulatorModeChanged(bool flag);
protected:
	struct Internal;
	sptr< Internal> mData;
};