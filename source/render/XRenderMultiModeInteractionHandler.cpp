#include "XRenderMultiModeInteractionHandler.h"
#include "CameraNavigationHandler.h"
#include "XManipulatorHandler.h"
#include "XRenderPickHandler.h"
#include "XRender.h"
#include "lib00_utilty/XUtilty.h"

XRenderMultiModeInteractionHandler::XRenderMultiModeInteractionHandler()
{
	mCameraHandler = makeShareDbObject<CameraNavigationHandler>();
	mPickHandler = makeShareDbObject<XRenderPickHandler>();
	mManipulatorHandler = makeShareDbObject<XManipulatorHandler>();
	mCurrentMode = (uint32_t)InteractMode::pick | 
								/*(uint32_t)InteractMode::manipulator|*/
								(uint32_t)InteractMode::camera;
}

XRenderMultiModeInteractionHandler::~XRenderMultiModeInteractionHandler()
{
}

void XRenderMultiModeInteractionHandler::LeftButtonPressEvent(XQ::Vec2i p, XQ::KeyboardModifier k)
{
	if(!isRenderActive())
		return;

	if(hasMode(InteractMode::camera))
		mCameraHandler->LeftButtonPressEvent(p,k);
	if(hasMode(InteractMode::pick))
		mPickHandler->LeftButtonPressEvent(p,k);
	if(hasMode(InteractMode::manipulator))
		mManipulatorHandler->LeftButtonPressEvent(p,k);
}

void XRenderMultiModeInteractionHandler::LeftButtonReleaseEvent(XQ::Vec2i p, XQ::KeyboardModifier k)
{
	if (!isRenderActive())
		return;

	if (hasMode(InteractMode::camera))
		mCameraHandler->LeftButtonReleaseEvent(p, k);
	if (hasMode(InteractMode::pick))
		mPickHandler->LeftButtonReleaseEvent(p, k);
	if (hasMode(InteractMode::manipulator))
		mManipulatorHandler->LeftButtonReleaseEvent(p, k);
}

void XRenderMultiModeInteractionHandler::MiddleButtonPressEvent(XQ::Vec2i p, XQ::KeyboardModifier k)
{
	if (!isRenderActive())
		return;

	if (hasMode(InteractMode::camera))
		mCameraHandler->MiddleButtonPressEvent(p, k);
	if (hasMode(InteractMode::pick))
		mPickHandler->MiddleButtonPressEvent(p, k);
	if (hasMode(InteractMode::manipulator))
		mManipulatorHandler->MiddleButtonPressEvent(p, k);
}

void XRenderMultiModeInteractionHandler::MiddleButtonReleaseEvent(XQ::Vec2i p, XQ::KeyboardModifier k)
{
	if (!isRenderActive())
		return;
	
	if (hasMode(InteractMode::camera))
		mCameraHandler->MiddleButtonReleaseEvent(p, k);
	if (hasMode(InteractMode::pick))
		mPickHandler->MiddleButtonReleaseEvent(p, k);
	if (hasMode(InteractMode::manipulator))
		mManipulatorHandler->MiddleButtonReleaseEvent(p, k);
}

void XRenderMultiModeInteractionHandler::RightButtonPressEvent(XQ::Vec2i, XQ::KeyboardModifier)
{
	if (!isRenderActive())
		return;
}

void XRenderMultiModeInteractionHandler::RightButtonReleaseEvent(XQ::Vec2i, XQ::KeyboardModifier)
{
	if (!isRenderActive())
		return;
}

void XRenderMultiModeInteractionHandler::EnterEvent()
{
	if (!isRenderActive())
		return;
}

void XRenderMultiModeInteractionHandler::LeaveEvent()
{
	if (!isRenderActive())
		return;
}

void XRenderMultiModeInteractionHandler::FoucsInEvent()
{
	if (!isRenderActive())
		return;
}

void XRenderMultiModeInteractionHandler::FoucsOutEvent()
{
	if (!isRenderActive())
		return;
}

void XRenderMultiModeInteractionHandler::ResizeEvent(XQ::Vec2i size)
{
	if (hasMode(InteractMode::camera))
		mCameraHandler->ResizeEvent(size);
	if (hasMode(InteractMode::manipulator))
		mManipulatorHandler->ResizeEvent(size);
	if (hasMode(InteractMode::pick))
		mPickHandler->ResizeEvent(size);
}

void XRenderMultiModeInteractionHandler::KeyPressEvent(XQ::Key, XQ::KeyboardModifier)
{
	if (!isRenderActive())
		return;
}

void XRenderMultiModeInteractionHandler::KeyReleaseEvent(XQ::Key, XQ::KeyboardModifier)
{
	if (!isRenderActive())
		return;
}

void XRenderMultiModeInteractionHandler::MouseMoveEvent(XQ::Vec2i p, XQ::KeyboardModifier k)
{
	if (!isRenderActive())
		return;

	mCameraHandler->MouseMoveEvent(p, k);
}

void XRenderMultiModeInteractionHandler::MouseWheelForwardEvent(XQ::Vec2i p, XQ::KeyboardModifier k)
{
	if (!isRenderActive())
		return;
	
	if (hasMode(InteractMode::camera))
		mCameraHandler->MouseWheelForwardEvent(p, k);
	if (hasMode(InteractMode::manipulator))
		mManipulatorHandler->MouseWheelForwardEvent(p, k);
}

void XRenderMultiModeInteractionHandler::MouseWheelBackwardEvent(XQ::Vec2i p, XQ::KeyboardModifier k)
{
	if (!isRenderActive())
		return;
	
	if (hasMode(InteractMode::camera))
		mCameraHandler->MouseWheelBackwardEvent(p, k);
	if (hasMode(InteractMode::manipulator))
		mManipulatorHandler->MouseWheelBackwardEvent(p, k);
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
}

void XRenderMultiModeInteractionHandler::setManipulatorHandler(sptr<XManipulatorHandler> manipulatorHandler)
{
	mManipulatorHandler = manipulatorHandler;
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

bool XRenderMultiModeInteractionHandler::hasMode(InteractMode mode)
{
	return (mCurrentMode & (uint32_t)mode) != 0;
}
