#include "XManipulatorHandler.h"
#include "XRender.h"
#include "lib00_utilty/XUtilty.h"
#include "lib02_camera/xcamera.h"
#include "XRenderCamera.h"


struct XManipulatorHandler::Internal {
	XQ::Vec2i mouseLstPos;
	MouseType mouseType = MouseType::none;
};

XManipulatorHandler::XManipulatorHandler():mData(new Internal)
{
}

XManipulatorHandler::~XManipulatorHandler()
{
}

void XManipulatorHandler::LeftButtonPressEvent(XQ::Vec2i windowpos, XQ::KeyboardModifier)
{
	if(!isRenderActive())
		return;
	
	mData->mouseLstPos = getRender()->window2render(windowpos);
	mData->mouseType = MouseType::left;
	std::cout << std::dec;
	XQ::print("CameraNavigationHandler LeftButtonPressEvent ", windowpos, mData->mouseLstPos);
}

void XManipulatorHandler::LeftButtonReleaseEvent(XQ::Vec2i windowpos, XQ::KeyboardModifier)
{
	if (!isRenderActive())
		return;

	mData->mouseType = MouseType::none;
	XQ::print("CameraNavigationHandler LeftButtonReleaseEvent");
}

void XManipulatorHandler::MiddleButtonPressEvent(XQ::Vec2i windowpos, XQ::KeyboardModifier)
{
	if (!isRenderActive())
		return;

	mData->mouseLstPos = getRender()->window2render(windowpos);
	mData->mouseType = MouseType::middle;
}

void XManipulatorHandler::MiddleButtonReleaseEvent(XQ::Vec2i windowpos, XQ::KeyboardModifier)
{
	if (!isRenderActive())
		return;
	mData->mouseType = MouseType::none;
}

void XManipulatorHandler::RightButtonPressEvent(XQ::Vec2i windowpos, XQ::KeyboardModifier)
{
	if (!isRenderActive())
		return;
	mData->mouseType = MouseType::right;
}

void XManipulatorHandler::RightButtonReleaseEvent(XQ::Vec2i windowpos, XQ::KeyboardModifier)
{
	if (!isRenderActive())
		return;
	mData->mouseType = MouseType::none;
}

void XManipulatorHandler::EnterEvent()
{
	if (!isRenderActive())
		return;
}

void XManipulatorHandler::LeaveEvent()
{
	if (!isRenderActive())
		return;
}

void XManipulatorHandler::FoucsInEvent()
{
	if (!isRenderActive())
		return;
}

void XManipulatorHandler::FoucsOutEvent()
{
	if (!isRenderActive())
		return;
}

void XManipulatorHandler::ResizeEvent(XQ::Vec2i)
{
	if (!isRenderActive())
		return;
}

void XManipulatorHandler::KeyPressEvent(XQ::Key, XQ::KeyboardModifier)
{
	if (!isRenderActive())
		return;
}

void XManipulatorHandler::KeyReleaseEvent(XQ::Key, XQ::KeyboardModifier)
{
	if (!isRenderActive())
		return;
}

void XManipulatorHandler::MouseMoveEvent(XQ::Vec2i windowpos, XQ::KeyboardModifier)
{
	if (!isRenderActive())
		return;
	if (mData->mouseType == MouseType::left) {
		//考虑相机旋转
		auto curpos = getRender()->window2render(windowpos);
		auto viewport = getRender()->getConvertViewPort();
		


		//更新位置
		mData->mouseLstPos = getRender()->window2render(windowpos);
	}
	else if (mData->mouseType == MouseType::middle) {
		//平移
		auto curpos = getRender()->window2render(windowpos);
		auto viewport = getRender()->getConvertViewPort();

		//更新位置
		mData->mouseLstPos = getRender()->window2render(windowpos);
	}
}

void XManipulatorHandler::MouseWheelForwardEvent(XQ::Vec2i windowpos, XQ::KeyboardModifier)
{
	if (!isRenderActive())
		return;
}

void XManipulatorHandler::MouseWheelBackwardEvent(XQ::Vec2i windowpos, XQ::KeyboardModifier)
{
	if (!isRenderActive())
		return;
}

bool XManipulatorHandler::isRenderActive() const
{
	auto render = getRender();
	if (render) {
		return render->isActive();
	}
	return false;
}

void XManipulatorHandler::setRender(sptr<XRender> render)
{
	XRenderInteractionEventHandler::setRender(render);
}

void XManipulatorHandler::slotRenderActiveChanged(bool active)
{
	if (active == false) {
		mData->mouseType = MouseType::none;
	}
}