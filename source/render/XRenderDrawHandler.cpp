#include "XRenderDrawHandler.h"
#include "XRender.h"
#include "lib00_utilty/XUtilty.h"
#include "lib02_camera/xcamera.h"
#include "XRenderCamera.h"
#include "lib04_opengl/XOpenGLFuntion.h"
#include "lib04_opengl/XOpenGLBuffer.h"
#include "lib04_opengl/XOpenGLFramebufferObject.h"
#include "xrendernode/XGeometryNode.h"
#include "xrendernode/datasource/xshapeSource.h"
#define GLOBALE_XAIS_NODE_NAME "__global_axis__"


struct XRenderDrawHandler::Internal {
	
};

XRenderDrawHandler::XRenderDrawHandler():mData(new Internal)
{
}

XRenderDrawHandler::~XRenderDrawHandler()
{
}


void XRenderDrawHandler::LeftButtonReleaseEvent(XQ::Vec2i windowpos, XQ::KeyboardModifier m, XEvent& event)
{
	if (event.isStopPropagate()) {
		return;
	}

	if (!isRenderActive())
		return;

}

void XRenderDrawHandler::MiddleButtonPressEvent(XQ::Vec2i windowpos, XQ::KeyboardModifier, XEvent& event)
{
	if (event.isStopPropagate()) {
		return;
	}

	if (!isRenderActive())
		return;

}

void XRenderDrawHandler::MiddleButtonReleaseEvent(XQ::Vec2i windowpos, XQ::KeyboardModifier, XEvent& event)
{
	if (event.isStopPropagate()) {
		return;
	}

	if (!isRenderActive())
		return;
}

void XRenderDrawHandler::RightButtonPressEvent(XQ::Vec2i windowpos, XQ::KeyboardModifier, XEvent& event)
{
	if (event.isStopPropagate()) {
		return;
	}

	if (!isRenderActive())
		return;

}

void XRenderDrawHandler::RightButtonReleaseEvent(XQ::Vec2i windowpos, XQ::KeyboardModifier, XEvent& event)
{
	if (event.isStopPropagate()) {
		return;
	}

	if (!isRenderActive())
		return;
}

void XRenderDrawHandler::LeftButtonDoublePressEvent(XQ::Vec2i windowpos, XQ::KeyboardModifier m, XEvent& event)
{
	if (event.isStopPropagate()) {
		return;
	}

	if (!isRenderActive())
		return;
}

void XRenderDrawHandler::RightButtonDoublePressEvent(XQ::Vec2i, XQ::KeyboardModifier, XEvent& event)
{

}

void XRenderDrawHandler::MiddleButtonDoublePressEvent(XQ::Vec2i, XQ::KeyboardModifier, XEvent& event)
{

}

void XRenderDrawHandler::EnterEvent(XEvent& event)
{
	if (event.isStopPropagate()) {
		return;
	}

	if (!isRenderActive())
		return;
}

void XRenderDrawHandler::LeaveEvent(XEvent& event)
{
	if (event.isStopPropagate()) {
		return;
	}

	if (!isRenderActive())
		return;
}

void XRenderDrawHandler::FoucsInEvent(XEvent& event)
{
	if (event.isStopPropagate()) {
		return;
	}

	if (!isRenderActive())
		return;
}

void XRenderDrawHandler::FoucsOutEvent(XEvent& event)
{
	if (event.isStopPropagate()) {
		return;
	}

	if (!isRenderActive())
		return;
}

void XRenderDrawHandler::ResizeEvent(XQ::Vec2i size, XEvent& event)
{
	if (event.isStopPropagate()) {
		return;
	}
}

void XRenderDrawHandler::KeyPressEvent(XQ::Key, XQ::KeyboardModifier, XEvent& event)
{
	if (event.isStopPropagate()) {
		return;
	}

	if (!isRenderActive())
		return;
}

void XRenderDrawHandler::KeyReleaseEvent(XQ::Key, XQ::KeyboardModifier, XEvent& event)
{
	if (event.isStopPropagate()) {
		return;
	}

	if (!isRenderActive())
		return;
}

void XRenderDrawHandler::LeftButtonPressEvent(XQ::Vec2i windowpos, XQ::KeyboardModifier m, XEvent& event)
{
	if (event.isStopPropagate()) {
		return;
	}

	if (!isRenderActive())
		return;
}

void XRenderDrawHandler::MouseMoveEvent(XQ::Vec2i windowpos, XQ::KeyboardModifier m, XEvent& event)
{
	if (event.isStopPropagate()) {
		return;
	}

	if (!isRenderActive())
		return;

}

void XRenderDrawHandler::MouseWheelForwardEvent(XQ::Vec2i windowpos, XQ::KeyboardModifier m, XEvent& event)
{
	if (event.isStopPropagate()) {
		return;
	}

	if (!isRenderActive())
		return;
}

void XRenderDrawHandler::MouseWheelBackwardEvent(XQ::Vec2i windowpos, XQ::KeyboardModifier m, XEvent& event)
{
	if (event.isStopPropagate()) {
		return;
	}

	if (!isRenderActive())
		return;
}

bool XRenderDrawHandler::isRenderActive() const
{
	auto render = getRender();
	if (render) {
		return render->isActive();
	}
	return false;
}

void XRenderDrawHandler::setRender(sptr<XRender> render)
{
	XRenderInteractionEventHandler::setRender(render);
}

void XRenderDrawHandler::slotRenderActiveChanged(bool active)
{
	if (active == false) {
		//mData->mouseType = MouseType::none;
	}
}

