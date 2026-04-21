#include "XInteractionEventHandler.h"
#include "xsignal/XSignal.h"
class XInteractionEventHandler::Internal {
public:
	~Internal() {
		mConnector.disconnect();
	}
	xsig::xconnector mConnector;
};

XInteractionEventHandler::XInteractionEventHandler():mData(new Internal)
{
}

XInteractionEventHandler::~XInteractionEventHandler()
{
}

void XInteractionEventHandler::Init()
{
	XDataBaseObject::Init();
	mData->mConnector.connect(this, &XInteractionEventHandler::SigLeftButtonPress, [this](XQ::Vec2i p, XQ::KeyboardModifier k)
	{XEvent event;this->LeftButtonPressEvent(p,k,event);});

	mData->mConnector.connect(this, &XInteractionEventHandler::SigLeftButtonRelease, [this](XQ::Vec2i p, XQ::KeyboardModifier k)
		{XEvent event; this->LeftButtonReleaseEvent(p, k, event); });

	mData->mConnector.connect(this, &XInteractionEventHandler::SigMiddleButtonPress, [this](XQ::Vec2i p, XQ::KeyboardModifier k)
		{XEvent event; this->MiddleButtonPressEvent(p, k, event); });

	mData->mConnector.connect(this, &XInteractionEventHandler::SigMiddleButtonRelease, [this](XQ::Vec2i p, XQ::KeyboardModifier k)
		{XEvent event; this->MiddleButtonReleaseEvent(p, k, event); });

	mData->mConnector.connect(this, &XInteractionEventHandler::SigRightButtonPress, [this](XQ::Vec2i p, XQ::KeyboardModifier k)
		{XEvent event; this->RightButtonPressEvent(p, k, event); });

	mData->mConnector.connect(this, &XInteractionEventHandler::SigRightButtonRelease, [this](XQ::Vec2i p, XQ::KeyboardModifier k)
		{XEvent event; this->RightButtonReleaseEvent(p, k, event); });

	mData->mConnector.connect(this, &XInteractionEventHandler::SigEnter, [this]() {
			XEvent event;
			this->EnterEvent(event);
		});

	mData->mConnector.connect(this, &XInteractionEventHandler::SigLeave, [this]() {
		XEvent event;
		this->LeaveEvent(event);
		});

	mData->mConnector.connect(this, &XInteractionEventHandler::SigFoucsIn, [this]() {
		XEvent event;
		this->FoucsInEvent(event);
		});

	mData->mConnector.connect(this, &XInteractionEventHandler::SigFoucsOut, [this]() {
		XEvent event;
		this->FoucsOutEvent(event);
		});

	mData->mConnector.connect(this, &XInteractionEventHandler::SigResize, [this](XQ::Vec2i p) {
		XEvent event;
		this->ResizeEvent(p,event);
		});

	mData->mConnector.connect(this, &XInteractionEventHandler::SigKeyPress, [this](XQ::Key k, XQ::KeyboardModifier m) {
		XEvent event;
		this->KeyPressEvent(k,m, event);
		});

	mData->mConnector.connect(this, &XInteractionEventHandler::SigKeyRelease, [this](XQ::Key k, XQ::KeyboardModifier m) {
		XEvent event;
		this->KeyReleaseEvent(k, m, event);
		});

	mData->mConnector.connect(this, &XInteractionEventHandler::SigMouseMove, [this](XQ::Vec2i p, XQ::KeyboardModifier k) {
		XEvent event;
		this->MouseMoveEvent(p, k, event);
		});

	mData->mConnector.connect(this, &XInteractionEventHandler::SigMouseWheelForward, [this](XQ::Vec2i p, XQ::KeyboardModifier k) {
		XEvent event;
		this->MouseWheelForwardEvent(p, k, event);
		});

	mData->mConnector.connect(this, &XInteractionEventHandler::SigMouseWheelBackward, [this](XQ::Vec2i p, XQ::KeyboardModifier k) {
		XEvent event;
		this->MouseWheelBackwardEvent(p, k, event);
		});

	mData->mConnector.connect(this, &XInteractionEventHandler::SigTimeOut, [this](int id, double ms) {
		XEvent event;
		this->TimeEvent(id,ms, event);
		});
}

void XInteractionEventHandler::LeftButtonPressEvent(XQ::Vec2i, XQ::KeyboardModifier, XEvent& event)
{
	
}

void XInteractionEventHandler::LeftButtonReleaseEvent(XQ::Vec2i, XQ::KeyboardModifier, XEvent& event)
{
}

void XInteractionEventHandler::MiddleButtonPressEvent(XQ::Vec2i, XQ::KeyboardModifier, XEvent& event)
{
}

void XInteractionEventHandler::MiddleButtonReleaseEvent(XQ::Vec2i, XQ::KeyboardModifier, XEvent& event)
{
}

void XInteractionEventHandler::RightButtonPressEvent(XQ::Vec2i, XQ::KeyboardModifier, XEvent& event)
{
}

void XInteractionEventHandler::RightButtonReleaseEvent(XQ::Vec2i, XQ::KeyboardModifier, XEvent& event)
{
}

void XInteractionEventHandler::EnterEvent(XEvent& event)
{
}

void XInteractionEventHandler::LeaveEvent(XEvent& event)
{
}

void XInteractionEventHandler::FoucsInEvent(XEvent& event)
{
}

void XInteractionEventHandler::FoucsOutEvent(XEvent& event)
{
}

void XInteractionEventHandler::ResizeEvent(XQ::Vec2i, XEvent& event)
{
}

void XInteractionEventHandler::KeyPressEvent(XQ::Key, XQ::KeyboardModifier, XEvent& event)
{
}

void XInteractionEventHandler::KeyReleaseEvent(XQ::Key, XQ::KeyboardModifier, XEvent& event)
{
}

void XInteractionEventHandler::MouseMoveEvent(XQ::Vec2i, XQ::KeyboardModifier, XEvent& event)
{
}

void XInteractionEventHandler::MouseWheelForwardEvent(XQ::Vec2i, XQ::KeyboardModifier, XEvent& event)
{
}

void XInteractionEventHandler::MouseWheelBackwardEvent(XQ::Vec2i, XQ::KeyboardModifier, XEvent& event)
{
}

void XInteractionEventHandler::TimeEvent(int id, double time, XEvent& event)
{
}
