#include "XInteractionEventHandler.h"
#include "xsignal/XSignal.h"

XInteractionEventHandler::XInteractionEventHandler()
{
}

XInteractionEventHandler::~XInteractionEventHandler()
{
}

void XInteractionEventHandler::Init()
{
	XDataBaseObject::Init();
	xsig::connect(this, &XInteractionEventHandler::SigLeftButtonPress, this, &XInteractionEventHandler::LeftButtonPressEvent);
	xsig::connect(this, &XInteractionEventHandler::SigLeftButtonRelease, this, &XInteractionEventHandler::LeftButtonReleaseEvent);
	xsig::connect(this, &XInteractionEventHandler::SigMiddleButtonPress, this, &XInteractionEventHandler::MiddleButtonPressEvent);
	xsig::connect(this, &XInteractionEventHandler::SigMiddleButtonRelease, this, &XInteractionEventHandler::MiddleButtonReleaseEvent);
	xsig::connect(this, &XInteractionEventHandler::SigRightButtonPress, this, &XInteractionEventHandler::RightButtonPressEvent);
	xsig::connect(this, &XInteractionEventHandler::SigRightButtonRelease, this, &XInteractionEventHandler::RightButtonReleaseEvent);
	xsig::connect(this, &XInteractionEventHandler::SigEnter, this, &XInteractionEventHandler::EnterEvent);
	xsig::connect(this, &XInteractionEventHandler::SigLeave, this, &XInteractionEventHandler::LeaveEvent);
	xsig::connect(this, &XInteractionEventHandler::SigFoucsIn, this, &XInteractionEventHandler::FoucsInEvent);
	xsig::connect(this, &XInteractionEventHandler::SigFoucsOut, this, &XInteractionEventHandler::FoucsOutEvent);
	xsig::connect(this, &XInteractionEventHandler::SigResize, this, &XInteractionEventHandler::ResizeEvent);
	xsig::connect(this, &XInteractionEventHandler::SigKeyPress, this, &XInteractionEventHandler::KeyPressEvent);
	xsig::connect(this, &XInteractionEventHandler::SigKeyRelease, this, &XInteractionEventHandler::KeyReleaseEvent);
	xsig::connect(this, &XInteractionEventHandler::SigMouseMove, this, &XInteractionEventHandler::MouseMoveEvent);
	xsig::connect(this, &XInteractionEventHandler::SigMouseWheelForward, this, &XInteractionEventHandler::MouseWheelForwardEvent);
	xsig::connect(this, &XInteractionEventHandler::SigMouseWheelBackward, this, &XInteractionEventHandler::MouseWheelBackwardEvent);
	xsig::connect(this, &XInteractionEventHandler::SigTimeOut, this, &XInteractionEventHandler::TimeEvent);
}

void XInteractionEventHandler::LeftButtonPressEvent(XQ::Vec2i, XQ::KeyboardModifier)
{
	
}

void XInteractionEventHandler::LeftButtonReleaseEvent(XQ::Vec2i, XQ::KeyboardModifier)
{
}

void XInteractionEventHandler::MiddleButtonPressEvent(XQ::Vec2i, XQ::KeyboardModifier)
{
}

void XInteractionEventHandler::MiddleButtonReleaseEvent(XQ::Vec2i, XQ::KeyboardModifier)
{
}

void XInteractionEventHandler::RightButtonPressEvent(XQ::Vec2i, XQ::KeyboardModifier)
{
}

void XInteractionEventHandler::RightButtonReleaseEvent(XQ::Vec2i, XQ::KeyboardModifier)
{
}

void XInteractionEventHandler::EnterEvent()
{
}

void XInteractionEventHandler::LeaveEvent()
{
}

void XInteractionEventHandler::FoucsInEvent()
{
}

void XInteractionEventHandler::FoucsOutEvent()
{
}

void XInteractionEventHandler::ResizeEvent(XQ::Vec2i)
{
}

void XInteractionEventHandler::KeyPressEvent(XQ::Key, XQ::KeyboardModifier)
{
}

void XInteractionEventHandler::KeyReleaseEvent(XQ::Key, XQ::KeyboardModifier)
{
}

void XInteractionEventHandler::MouseMoveEvent(XQ::Vec2i, XQ::KeyboardModifier)
{
}

void XInteractionEventHandler::MouseWheelForwardEvent(XQ::Vec2i, XQ::KeyboardModifier)
{
}

void XInteractionEventHandler::MouseWheelBackwardEvent(XQ::Vec2i, XQ::KeyboardModifier)
{
}

void XInteractionEventHandler::TimeEvent(int id, double time)
{
}
