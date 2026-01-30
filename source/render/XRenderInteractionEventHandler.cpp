#include "XRenderInteractionEventHandler.h"
#include "XRender.h"
#include "lib00_utilty/XUtilty.h"

XRenderInteractionEventHandler::XRenderInteractionEventHandler()
{
}

XRenderInteractionEventHandler::~XRenderInteractionEventHandler()
{
}

void XRenderInteractionEventHandler::LeftButtonPressEvent(XQ::Vec2i, XQ::KeyboardModifier)
{
	if(!isRenderActive())
		return;

	//XQ::print("Render LeftButtonPressEvent");
}

void XRenderInteractionEventHandler::LeftButtonReleaseEvent(XQ::Vec2i, XQ::KeyboardModifier)
{
	if (!isRenderActive())
		return;

	//XQ::print("Render LeftButtonReleaseEvent");
}

void XRenderInteractionEventHandler::MiddleButtonPressEvent(XQ::Vec2i, XQ::KeyboardModifier)
{
	if (!isRenderActive())
		return;

	//XQ::print("Render MiddleButtonPressEvent");
}

void XRenderInteractionEventHandler::MiddleButtonReleaseEvent(XQ::Vec2i, XQ::KeyboardModifier)
{
	if (!isRenderActive())
		return;

	//XQ::print("Render MiddleButtonReleaseEvent");
}

void XRenderInteractionEventHandler::RightButtonPressEvent(XQ::Vec2i, XQ::KeyboardModifier)
{
	if (!isRenderActive())
		return;

	//XQ::print("Render RightButtonPressEvent");
}

void XRenderInteractionEventHandler::RightButtonReleaseEvent(XQ::Vec2i, XQ::KeyboardModifier)
{
	if (!isRenderActive())
		return;

	//XQ::print("Render RightButtonReleaseEvent");
}

void XRenderInteractionEventHandler::EnterEvent()
{
	if (!isRenderActive())
		return;

	//XQ::print("Render EnterEvent");
}

void XRenderInteractionEventHandler::LeaveEvent()
{
	if (!isRenderActive())
		return;

	//XQ::print("Render LeaveEvent");
}

void XRenderInteractionEventHandler::FoucsInEvent()
{
	if (!isRenderActive())
		return;
	//XQ::print("Render FoucsInEvent");
}

void XRenderInteractionEventHandler::FoucsOutEvent()
{
	if (!isRenderActive())
		return;
	//XQ::print("Render FoucsOutEvent");
}

void XRenderInteractionEventHandler::ResizeEvent(XQ::Vec2i)
{
	if (!isRenderActive())
		return;
	//XQ::print("Render ResizeEvent");
}

void XRenderInteractionEventHandler::KeyPressEvent(XQ::Key, XQ::KeyboardModifier)
{
	if (!isRenderActive())
		return;
	//XQ::print("Render KeyPressEvent");
}

void XRenderInteractionEventHandler::KeyReleaseEvent(XQ::Key, XQ::KeyboardModifier)
{
	if (!isRenderActive())
		return;
	//XQ::print("Render KeyReleaseEvent");
}

void XRenderInteractionEventHandler::MouseMoveEvent(XQ::Vec2i, XQ::KeyboardModifier)
{
	if (!isRenderActive())
		return;
	//XQ::print("Render MouseMoveEvent");
}

void XRenderInteractionEventHandler::MouseWheelForwardEvent(XQ::Vec2i, XQ::KeyboardModifier)
{
	if (!isRenderActive())
		return;
	//XQ::print("Render MouseWheelForwardEvent");
}

void XRenderInteractionEventHandler::MouseWheelBackwardEvent(XQ::Vec2i, XQ::KeyboardModifier)
{
	if (!isRenderActive())
		return;
	//XQ::print("Render MouseWheelBackwardEvent");
}

bool XRenderInteractionEventHandler::isRenderActive() const
{
	auto render = getRender();
	if (render) {
		return render->isActive();
	}
	return false;
}

void XRenderInteractionEventHandler::setRender(sptr<XRender> render)
{
	m_render = render;
	//ÐÅºÅ°ó¶¨
	if (auto attr = render->getAttribute("AttrActive")) {
		xsig::connect(attr,&XDataAttribute::sigAttrChanged,[this](sptr<XDataAttribute> attr, XDataChangeType type){
			if (type == XDataChangeType::ItemDataModified) {
				if (auto a = attr->asDerived<XAttr_Bool>()) {
					bool flag = a->getValue();
					slotRenderActiveChanged(flag);
				}
			}
		});
	}
}

sptr<XRender> XRenderInteractionEventHandler::getRender() const
{
	return m_render.lock();
}
