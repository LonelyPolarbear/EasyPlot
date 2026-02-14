#include "XRenderPickHandler.h"
#include "XRender.h"
#include "lib00_utilty/XUtilty.h"
#include "lib02_camera/xcamera.h"
#include "XRenderCamera.h"
#include "XViewPicker3D.h"

struct XRenderPickHandler::Internal {
	XQ::Vec2i mouseLstPos;
	MouseType mouseType = MouseType::none;
	sptr<XViewPicker3D> pick3d;
};

XRenderPickHandler::XRenderPickHandler():mData(new Internal)
{
	mData->pick3d = makeShareDbObject<XViewPicker3D>();
}

XRenderPickHandler::~XRenderPickHandler()
{
}

void XRenderPickHandler::LeftButtonPressEvent(XQ::Vec2i windowpos, XQ::KeyboardModifier)
{
	if(!isRenderActive())
		return;
	
	mData->mouseLstPos = getRender()->window2render(windowpos);
	mData->mouseType = MouseType::left;
	std::cout << std::dec;
	XQ::print("XRenderPickHandler LeftButtonPressEvent ", windowpos, mData->mouseLstPos);

	auto select=mData->pick3d->getPointSelection(windowpos);

	//获取拾取到的结果
	if(select.objectId!= 0)
		std::cout<<"objectId:"<<select.objectId <<" primitiveId:" << select.primitiveId << std::endl;
}

void XRenderPickHandler::LeftButtonReleaseEvent(XQ::Vec2i windowpos, XQ::KeyboardModifier)
{
	if (!isRenderActive())
		return;

	mData->mouseType = MouseType::none;
	XQ::print("XRenderPickHandler LeftButtonReleaseEvent");
}

void XRenderPickHandler::MiddleButtonPressEvent(XQ::Vec2i windowpos, XQ::KeyboardModifier)
{
	if (!isRenderActive())
		return;

	mData->mouseLstPos = getRender()->window2render(windowpos);
	mData->mouseType = MouseType::middle;
}

void XRenderPickHandler::MiddleButtonReleaseEvent(XQ::Vec2i windowpos, XQ::KeyboardModifier)
{
	if (!isRenderActive())
		return;
	mData->mouseType = MouseType::none;
}

void XRenderPickHandler::RightButtonPressEvent(XQ::Vec2i windowpos, XQ::KeyboardModifier)
{
	if (!isRenderActive())
		return;
	mData->mouseType = MouseType::right;
}

void XRenderPickHandler::RightButtonReleaseEvent(XQ::Vec2i windowpos, XQ::KeyboardModifier)
{
	if (!isRenderActive())
		return;
	mData->mouseType = MouseType::none;
}

void XRenderPickHandler::EnterEvent()
{
	if (!isRenderActive())
		return;
}

void XRenderPickHandler::LeaveEvent()
{
	if (!isRenderActive())
		return;
}

void XRenderPickHandler::FoucsInEvent()
{
	if (!isRenderActive())
		return;
}

void XRenderPickHandler::FoucsOutEvent()
{
	if (!isRenderActive())
		return;
}

void XRenderPickHandler::ResizeEvent(XQ::Vec2i size)
{
	if (mData->pick3d)
		mData->pick3d->slotRenderSizeChanged(size);
}

void XRenderPickHandler::KeyPressEvent(XQ::Key, XQ::KeyboardModifier)
{
	if (!isRenderActive())
		return;
}

void XRenderPickHandler::KeyReleaseEvent(XQ::Key, XQ::KeyboardModifier)
{
	if (!isRenderActive())
		return;
}

void XRenderPickHandler::MouseMoveEvent(XQ::Vec2i windowpos, XQ::KeyboardModifier)
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

void XRenderPickHandler::MouseWheelForwardEvent(XQ::Vec2i windowpos, XQ::KeyboardModifier)
{
	if (!isRenderActive())
		return;
}

void XRenderPickHandler::MouseWheelBackwardEvent(XQ::Vec2i windowpos, XQ::KeyboardModifier)
{
	if (!isRenderActive())
		return;
}

bool XRenderPickHandler::isRenderActive() const
{
	auto render = getRender();
	if (render) {
		return render->isActive();
	}
	return false;
}

void XRenderPickHandler::setRender(sptr<XRender> render)
{
	XRenderInteractionEventHandler::setRender(render);
	if (mData->pick3d)
		mData->pick3d->setRender(render);
}

void XRenderPickHandler::slotRenderActiveChanged(bool active)
{
	if (active == false) {
		mData->mouseType = MouseType::none;
	}
}