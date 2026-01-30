#include "CameraNavigationHandler.h"
#include "XRender.h"
#include "lib00_utilty/XUtilty.h"
#include "lib02_camera/xcamera.h"
#include "XRenderCamera.h"

enum class MouseType {
	none,
	left,
	right,
	middle
};
struct CameraNavigationHandler::Internal {
	XQ::Vec2i mouseLstPos;
	MouseType mouseType = MouseType::none;
};
CameraNavigationHandler::CameraNavigationHandler():mData(new Internal)
{
}

CameraNavigationHandler::~CameraNavigationHandler()
{
}

void CameraNavigationHandler::LeftButtonPressEvent(XQ::Vec2i windowpos, XQ::KeyboardModifier)
{
	if(!isRenderActive())
		return;
	
	mData->mouseLstPos = getRender()->window2render(windowpos);
	mData->mouseType = MouseType::left;
	std::cout << std::dec;
	XQ::print("CameraNavigationHandler LeftButtonPressEvent ", windowpos, mData->mouseLstPos);
}

void CameraNavigationHandler::LeftButtonReleaseEvent(XQ::Vec2i windowpos, XQ::KeyboardModifier)
{
	if (!isRenderActive())
		return;

	mData->mouseType = MouseType::none;
	XQ::print("CameraNavigationHandler LeftButtonReleaseEvent");
}

void CameraNavigationHandler::MiddleButtonPressEvent(XQ::Vec2i windowpos, XQ::KeyboardModifier)
{
	if (!isRenderActive())
		return;

	mData->mouseLstPos = getRender()->window2render(windowpos);
	mData->mouseType = MouseType::middle;
}

void CameraNavigationHandler::MiddleButtonReleaseEvent(XQ::Vec2i windowpos, XQ::KeyboardModifier)
{
	if (!isRenderActive())
		return;
	mData->mouseType = MouseType::none;
}

void CameraNavigationHandler::RightButtonPressEvent(XQ::Vec2i windowpos, XQ::KeyboardModifier)
{
	if (!isRenderActive())
		return;
	mData->mouseType = MouseType::right;
}

void CameraNavigationHandler::RightButtonReleaseEvent(XQ::Vec2i windowpos, XQ::KeyboardModifier)
{
	if (!isRenderActive())
		return;
	mData->mouseType = MouseType::none;
}

void CameraNavigationHandler::EnterEvent()
{
	if (!isRenderActive())
		return;
}

void CameraNavigationHandler::LeaveEvent()
{
	if (!isRenderActive())
		return;
}

void CameraNavigationHandler::FoucsInEvent()
{
	if (!isRenderActive())
		return;
}

void CameraNavigationHandler::FoucsOutEvent()
{
	if (!isRenderActive())
		return;
}

void CameraNavigationHandler::ResizeEvent(XQ::Vec2i)
{
	if (!isRenderActive())
		return;
}

void CameraNavigationHandler::KeyPressEvent(XQ::Key, XQ::KeyboardModifier)
{
	if (!isRenderActive())
		return;
}

void CameraNavigationHandler::KeyReleaseEvent(XQ::Key, XQ::KeyboardModifier)
{
	if (!isRenderActive())
		return;
}

void CameraNavigationHandler::MouseMoveEvent(XQ::Vec2i windowpos, XQ::KeyboardModifier)
{
	if (!isRenderActive())
		return;
	if (mData->mouseType == MouseType::left) {
		//考虑相机旋转
		auto curpos = getRender()->window2render(windowpos);
		auto viewport = getRender()->getConvertViewPort();
		
		getRender()->getCamera()->rotate(curpos,mData->mouseLstPos,viewport[2],viewport[3]);

		//更新位置
		mData->mouseLstPos = getRender()->window2render(windowpos);
	}
	else if (mData->mouseType == MouseType::middle) {
		//平移
		auto curpos = getRender()->window2render(windowpos);
		auto viewport = getRender()->getConvertViewPort();

		getRender()->getCamera()->translate(curpos, mData->mouseLstPos, viewport[2], viewport[3]);

		//更新位置
		mData->mouseLstPos = getRender()->window2render(windowpos);
	}
}

void CameraNavigationHandler::MouseWheelForwardEvent(XQ::Vec2i windowpos, XQ::KeyboardModifier)
{
	if (!isRenderActive())
		return;
	
	double factor = 1.1;
	auto c = getRender()->getCamera();
	c->scale(factor);
}

void CameraNavigationHandler::MouseWheelBackwardEvent(XQ::Vec2i windowpos, XQ::KeyboardModifier)
{
	if (!isRenderActive())
		return;

	double factor = 1/1.1;
	auto c = getRender()->getCamera();
	c->scale(factor);
}

bool CameraNavigationHandler::isRenderActive() const
{
	auto render = getRender();
	if (render) {
		return render->isActive();
	}
	return false;
}

void CameraNavigationHandler::setRender(sptr<XRender> render)
{
	XRenderInteractionEventHandler::setRender(render);
}

void CameraNavigationHandler::slotRenderActiveChanged(bool active)
{
	if (active == false) {
		mData->mouseType = MouseType::none;
	}
}