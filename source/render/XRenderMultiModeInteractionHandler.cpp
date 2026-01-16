#include "XRenderMultiModeInteractionHandler.h"
#include "CameraNavigationHandler.h"
#include "XRender.h"
#include "lib00_utilty/XUtilty.h"

XRenderMultiModeInteractionHandler::XRenderMultiModeInteractionHandler()
{
	mCameraHandler = makeShareDbObject<CameraNavigationHandler>();
}

XRenderMultiModeInteractionHandler::~XRenderMultiModeInteractionHandler()
{
}

void XRenderMultiModeInteractionHandler::LeftButtonPressEvent(XQ::Vec2i p, XQ::KeyboardModifier k)
{
	if(!isRenderActive())
		return;

	mCameraHandler->LeftButtonPressEvent(p,k);
}

void XRenderMultiModeInteractionHandler::LeftButtonReleaseEvent(XQ::Vec2i p, XQ::KeyboardModifier k)
{
	if (!isRenderActive())
		return;

	mCameraHandler->LeftButtonReleaseEvent(p, k);
}

void XRenderMultiModeInteractionHandler::MiddleButtonPressEvent(XQ::Vec2i p, XQ::KeyboardModifier k)
{
	if (!isRenderActive())
		return;

	mCameraHandler->MiddleButtonPressEvent(p, k);
}

void XRenderMultiModeInteractionHandler::MiddleButtonReleaseEvent(XQ::Vec2i p, XQ::KeyboardModifier k)
{
	if (!isRenderActive())
		return;
	mCameraHandler->MiddleButtonReleaseEvent(p, k);
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

void XRenderMultiModeInteractionHandler::ResizeEvent(XQ::Vec2i)
{
	if (!isRenderActive())
		return;
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
	mCameraHandler->MouseWheelForwardEvent(p, k);
}

void XRenderMultiModeInteractionHandler::MouseWheelBackwardEvent(XQ::Vec2i p, XQ::KeyboardModifier k)
{
	if (!isRenderActive())
		return;
	mCameraHandler->MouseWheelBackwardEvent(p, k);
}

void XRenderMultiModeInteractionHandler::setRender(sptr<XRender> render)
{
	XRenderInteractionEventHandler::setRender(render);
	mCameraHandler->setRender(render);
}

void XRenderMultiModeInteractionHandler::setCameraNavigationHandler(sptr<CameraNavigationHandler> cameraHandler)
{
	mCameraHandler = cameraHandler;
}

sptr<CameraNavigationHandler> XRenderMultiModeInteractionHandler::getCameraNavigationHandler() const
{
	return mCameraHandler;
}
