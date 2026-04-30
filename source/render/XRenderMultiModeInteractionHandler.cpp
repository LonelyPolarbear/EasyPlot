#include "XRenderMultiModeInteractionHandler.h"
#include "CameraNavigationHandler.h"
#include "XManipulatorHandler.h"
#include "XRenderPickHandler.h"
#include "XRender.h"
#include "lib00_utilty/XUtilty.h"
#include <xsignal/XSignal.h>
#include <xlog/XLogger.h>
class XRenderMultiModeInteractionHandler::Internal {
public:
	xsig::xconnector mConnector;
};

XRenderMultiModeInteractionHandler::XRenderMultiModeInteractionHandler() :mData(new Internal)
{
	auto CameraHandler = makeShareDbObject<CameraNavigationHandler>();
	auto PickHandler = makeShareDbObject<XRenderPickHandler>();
	auto ManipulatorHandler = makeShareDbObject<XManipulatorHandler>();
	
	setPickHandler(PickHandler);
	setCameraNavigationHandler(CameraHandler);
	setManipulatorHandler(ManipulatorHandler);

	mCurrentMode = XQ::InteractMode::pick | XQ::InteractMode::camera;
	SigInteractModeChange(mCurrentMode);
}

XRenderMultiModeInteractionHandler::~XRenderMultiModeInteractionHandler()
{
}

void XRenderMultiModeInteractionHandler::LeftButtonPressEvent(XQ::Vec2i p, XQ::KeyboardModifier k, XEvent& event)
{
	if(!isRenderActive())
		return;

	if(hasMode(XQ::InteractMode::pick))
		mPickHandler->LeftButtonPressEvent(p,k,event);
	
	if(hasMode(XQ::InteractMode::manipulator))
		mManipulatorHandler->LeftButtonPressEvent(p,k, event);

	if (hasMode(XQ::InteractMode::camera))
		mCameraHandler->LeftButtonPressEvent(p, k, event);
	//调用顺序 不要随意更换，这样可以保证拾取优先
}

void XRenderMultiModeInteractionHandler::LeftButtonReleaseEvent(XQ::Vec2i p, XQ::KeyboardModifier k, XEvent& event)
{
	if (!isRenderActive())
		return;

	if (hasMode(XQ::InteractMode::pick))
		mPickHandler->LeftButtonReleaseEvent(p, k, event);
	if (hasMode(XQ::InteractMode::manipulator))
		mManipulatorHandler->LeftButtonReleaseEvent(p, k, event);
	if (hasMode(XQ::InteractMode::camera))
		mCameraHandler->LeftButtonReleaseEvent(p, k, event);
}

void XRenderMultiModeInteractionHandler::MiddleButtonPressEvent(XQ::Vec2i p, XQ::KeyboardModifier k, XEvent& event)
{
	if (!isRenderActive())
		return;

	if (hasMode(XQ::InteractMode::pick))
		mPickHandler->MiddleButtonPressEvent(p, k, event);
	if (hasMode(XQ::InteractMode::manipulator))
		mManipulatorHandler->MiddleButtonPressEvent(p, k, event);
	if (hasMode(XQ::InteractMode::camera))
		mCameraHandler->MiddleButtonPressEvent(p, k, event);
}

void XRenderMultiModeInteractionHandler::MiddleButtonReleaseEvent(XQ::Vec2i p, XQ::KeyboardModifier k, XEvent& event)
{
	if (!isRenderActive())
		return;
	
	if (hasMode(XQ::InteractMode::pick))
		mPickHandler->MiddleButtonReleaseEvent(p, k, event);
	if (hasMode(XQ::InteractMode::manipulator))
		mManipulatorHandler->MiddleButtonReleaseEvent(p, k, event);
	if (hasMode(XQ::InteractMode::camera))
		mCameraHandler->MiddleButtonReleaseEvent(p, k, event);
}

void XRenderMultiModeInteractionHandler::RightButtonPressEvent(XQ::Vec2i, XQ::KeyboardModifier, XEvent& event)
{
	if (!isRenderActive())
		return;
}

void XRenderMultiModeInteractionHandler::RightButtonReleaseEvent(XQ::Vec2i, XQ::KeyboardModifier, XEvent& event)
{
	if (!isRenderActive())
		return;
}

void XRenderMultiModeInteractionHandler::EnterEvent(XEvent& event)
{
	if (!isRenderActive())
		return;
}

void XRenderMultiModeInteractionHandler::LeaveEvent(XEvent& event)
{
	if (!isRenderActive())
		return;
}

void XRenderMultiModeInteractionHandler::FoucsInEvent(XEvent& event)
{
	if (!isRenderActive())
		return;
}

void XRenderMultiModeInteractionHandler::FoucsOutEvent(XEvent& event)
{
	if (!isRenderActive())
		return;
}

void XRenderMultiModeInteractionHandler::ResizeEvent(XQ::Vec2i size, XEvent& event)
{
	if (hasMode(XQ::InteractMode::manipulator))
		mManipulatorHandler->ResizeEvent(size,event);
	if (hasMode(XQ::InteractMode::pick))
		mPickHandler->ResizeEvent(size, event);
	if (hasMode(XQ::InteractMode::camera))
		mCameraHandler->ResizeEvent(size, event);
}

void XRenderMultiModeInteractionHandler::KeyPressEvent(XQ::Key key, XQ::KeyboardModifier modifier, XEvent& event)
{
	if (!isRenderActive())
		return;
	if (key == XQ::Key::Key_E &&  (modifier & XQ::KeyboardModifier::ControlModifier)) {
		//进入操作柄模式
		mCurrentMode = mCurrentMode | XQ::InteractMode::manipulator;
		SigInteractModeChange(mCurrentMode);
		XLOG_INFO("Joystick mode activated!");
	}
	if (key == XQ::Key::Key_Escape) {
		if (hasMode(XQ::InteractMode::manipulator)) {
			mCurrentMode = mCurrentMode ^ XQ::InteractMode::manipulator;
			SigInteractModeChange(mCurrentMode);
			XLOG_INFO("Exit joystick mode!");
		}
	}
}

void XRenderMultiModeInteractionHandler::KeyReleaseEvent(XQ::Key, XQ::KeyboardModifier, XEvent& event)
{
	if (!isRenderActive())
		return;
}

void XRenderMultiModeInteractionHandler::MouseMoveEvent(XQ::Vec2i p, XQ::KeyboardModifier k, XEvent& event)
{
	if (!isRenderActive())
		return;
	
	if(hasMode(XQ::InteractMode::manipulator))
		mManipulatorHandler->MouseMoveEvent(p, k, event);
	if (hasMode(XQ::InteractMode::pick))
		mPickHandler->MouseMoveEvent(p, k, event);
	if (hasMode(XQ::InteractMode::camera))
		mCameraHandler->MouseMoveEvent(p, k, event);
}

void XRenderMultiModeInteractionHandler::MouseWheelForwardEvent(XQ::Vec2i p, XQ::KeyboardModifier k, XEvent& event)
{
	if (!isRenderActive())
		return;
	
	if (hasMode(XQ::InteractMode::manipulator))
		mManipulatorHandler->MouseWheelForwardEvent(p, k, event);
	if (hasMode(XQ::InteractMode::camera))
		mCameraHandler->MouseWheelForwardEvent(p, k, event);
}

void XRenderMultiModeInteractionHandler::MouseWheelBackwardEvent(XQ::Vec2i p, XQ::KeyboardModifier k, XEvent& event)
{
	if (!isRenderActive())
		return;
	
	if (hasMode(XQ::InteractMode::manipulator))
		mManipulatorHandler->MouseWheelBackwardEvent(p, k, event);
	if (hasMode(XQ::InteractMode::camera))
		mCameraHandler->MouseWheelBackwardEvent(p, k, event);
}

void XRenderMultiModeInteractionHandler::setRender(sptr<XRender> render)
{
	XRenderInteractionEventHandler::setRender(render);
	mCameraHandler->setRender(render);
	mPickHandler->setRender(render);
	mManipulatorHandler->setRender(render);
}

void XRenderMultiModeInteractionHandler::setCameraNavigationHandler(sptr<CameraNavigationHandler> cameraHandler)
{
	mCameraHandler = cameraHandler;
}

void XRenderMultiModeInteractionHandler::setPickHandler(sptr<XRenderPickHandler> pickHandler)
{
	mPickHandler = pickHandler;
	mData->mConnector.disconnect("PickHandler SigRenderNodeSelected");
	mData->mConnector.connect(pickHandler,&XRenderPickHandler::SigRenderNodeSelected,this, &XRenderMultiModeInteractionHandler::SigRenderNodeSelected,"PickHandler SigRenderNodeSelected");
}

void XRenderMultiModeInteractionHandler::setManipulatorHandler(sptr<XManipulatorHandler> manipulatorHandler)
{
	mManipulatorHandler = manipulatorHandler;
	mData->mConnector.disconnect("ManipulatorHandler SigRenderNodeSelected");
	mData->mConnector.disconnect("SigInteractModeChange");
	mData->mConnector.connect
	(this, &XRenderMultiModeInteractionHandler::SigRenderNodeSelected, manipulatorHandler,&XManipulatorHandler::SigRenderNodeHasSelected,"ManipulatorHandler SigRenderNodeSelected");

	mData->mConnector.connect(
		this, &XRenderMultiModeInteractionHandler::SigInteractModeChange, [this](XQ::InteractMode mode) {
			if (hasMode(XQ::InteractMode::manipulator)) {
				mManipulatorHandler->SigManipulatorModeChanged(true);
			}
			else {
				mManipulatorHandler->SigManipulatorModeChanged(false);
			}
		}
	,"SigInteractModeChange");
}

sptr<CameraNavigationHandler> XRenderMultiModeInteractionHandler::getCameraNavigationHandler() const
{
	return mCameraHandler;
}

sptr<XRenderPickHandler> XRenderMultiModeInteractionHandler::getPickHandler() const
{
	return mPickHandler;
}

sptr<XManipulatorHandler> XRenderMultiModeInteractionHandler::getManipulatorHandler() const
{
	return mManipulatorHandler;
}

bool XRenderMultiModeInteractionHandler::hasMode(XQ::InteractMode mode)
{
	using T = std::underlying_type_t<XQ::InteractMode>;
	T v = static_cast<T>(mode);
	return (v & static_cast<T>(mCurrentMode)) == v;
}

void XRenderMultiModeInteractionHandler::setMode(XQ::InteractMode mode)
{
	mCurrentMode = mode;
}

XQ::InteractMode XRenderMultiModeInteractionHandler::getMode() const
{
	return mCurrentMode;
}
