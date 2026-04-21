#include "XManipulatorHandler.h"
#include "XRender.h"
#include "lib00_utilty/XUtilty.h"
#include "lib02_camera/xcamera.h"
#include "XRenderCamera.h"
#include <xsignal/XSignal.h>
#include <lib05_shape/renderNode3d/XTransformGizmoRenderNode.h>
#include <xlog/XLogger.h>

struct XManipulatorHandler::Internal {
	XQ::Vec2i mouseLstPos;
	MouseType mouseType = MouseType::none;
	xsig::xconnector mConnector;
	bool enable = false;
	~Internal() {
		mConnector.disconnect();
	}

	sptr<XTransformGizmoRenderNode> mManipulatorNode;

	XTransformGizmoRenderNode::InteractObjectType  mIntaractType= XTransformGizmoRenderNode::InteractObjectType::none;

	sptr<XTransformGizmoRenderNode> getGizmoRenderNode() {
		if (!mManipulatorNode) {
			mManipulatorNode = makeShareDbObject<XTransformGizmoRenderNode>();
		}
		return mManipulatorNode;
	}
};

XManipulatorHandler::XManipulatorHandler() :mData(new Internal)
{
	mData->mConnector.connect(this, &XManipulatorHandler::SigRenderNodeHasSelected, this, &XManipulatorHandler::slotRenderNodeHasSelected);
	mData->mConnector.connect(this, &XManipulatorHandler::SigManipulatorModeChanged, this, &XManipulatorHandler::slotManipulatorModeChanged);
}

XManipulatorHandler::~XManipulatorHandler()
{
}

void XManipulatorHandler::LeftButtonPressEvent(XQ::Vec2i windowpos, XQ::KeyboardModifier, XEvent& event)
{
	if (event.isStopPropagate()) {
		return;
	}

	if (!isRenderActive())
		return;

	mData->mouseLstPos = getRender()->window2render(windowpos);
	mData->mouseType = MouseType::left;
	std::cout << std::dec;
	XQ::print("CameraNavigationHandler LeftButtonPressEvent ", windowpos, mData->mouseLstPos);
}

void XManipulatorHandler::LeftButtonReleaseEvent(XQ::Vec2i windowpos, XQ::KeyboardModifier, XEvent& event)
{
	if (event.isStopPropagate()) {
		return;
	}

	if (!isRenderActive())
		return;

	mData->mouseType = MouseType::none;
	XQ::print("CameraNavigationHandler LeftButtonReleaseEvent");
}

void XManipulatorHandler::MiddleButtonPressEvent(XQ::Vec2i windowpos, XQ::KeyboardModifier, XEvent& event)
{
	if (event.isStopPropagate()) {
		return;
	}

	if (!isRenderActive())
		return;

	mData->mouseLstPos = getRender()->window2render(windowpos);
	mData->mouseType = MouseType::middle;
}

void XManipulatorHandler::MiddleButtonReleaseEvent(XQ::Vec2i windowpos, XQ::KeyboardModifier, XEvent& event)
{
	if (event.isStopPropagate()) {
		return;
	}

	if (!isRenderActive())
		return;
	mData->mouseType = MouseType::none;
}

void XManipulatorHandler::RightButtonPressEvent(XQ::Vec2i windowpos, XQ::KeyboardModifier, XEvent& event)
{
	if (event.isStopPropagate()) {
		return;
	}

	if (!isRenderActive())
		return;
	mData->mouseType = MouseType::right;
}

void XManipulatorHandler::RightButtonReleaseEvent(XQ::Vec2i windowpos, XQ::KeyboardModifier, XEvent& event)
{
	if (event.isStopPropagate()) {
		return;
	}

	if (!isRenderActive())
		return;
	mData->mouseType = MouseType::none;
}

void XManipulatorHandler::EnterEvent(XEvent& event)
{
	if (event.isStopPropagate()) {
		return;
	}

	if (!isRenderActive())
		return;
}

void XManipulatorHandler::LeaveEvent(XEvent& event)
{
	if (event.isStopPropagate()) {
		return;
	}

	if (!isRenderActive())
		return;
}

void XManipulatorHandler::FoucsInEvent(XEvent& event)
{
	if (event.isStopPropagate()) {
		return;
	}

	if (!isRenderActive())
		return;
}

void XManipulatorHandler::FoucsOutEvent(XEvent& event)
{
	if (event.isStopPropagate()) {
		return;
	}

	if (!isRenderActive())
		return;
}

void XManipulatorHandler::ResizeEvent(XQ::Vec2i, XEvent& event)
{
	if (event.isStopPropagate()) {
		return;
	}

	if (!isRenderActive())
		return;
}

void XManipulatorHandler::KeyPressEvent(XQ::Key, XQ::KeyboardModifier, XEvent& event)
{
	if (event.isStopPropagate()) {
		return;
	}

	if (!isRenderActive())
		return;
}

void XManipulatorHandler::KeyReleaseEvent(XQ::Key, XQ::KeyboardModifier, XEvent& event)
{
	if (event.isStopPropagate()) {
		return;
	}

	if (!isRenderActive())
		return;
}

void XManipulatorHandler::MouseMoveEvent(XQ::Vec2i windowpos, XQ::KeyboardModifier, XEvent& event)
{
	if (event.isStopPropagate()) {
		return;
	}

	if (!isRenderActive())
		return;
	if (!mData->enable) {
		return;
	}
	if (mData->mouseType != MouseType::left) {
		return;
	}

	if(mData->mIntaractType	== XTransformGizmoRenderNode::InteractObjectType::none)
		return;

	//在鼠标摁下，并且移动的时候，且设置了正确的交互行为，则开始交互设置位姿

	auto curpos = getRender()->window2render(windowpos);
	auto viewport = getRender()->getConvertViewPort();

	//更新位置
	mData->mouseLstPos = getRender()->window2render(windowpos);
	event.stopPropagate();

}

void XManipulatorHandler::MouseWheelForwardEvent(XQ::Vec2i windowpos, XQ::KeyboardModifier, XEvent& event)
{
	if (event.isStopPropagate()) {
		return;
	}

	if (!isRenderActive())
		return;
}

void XManipulatorHandler::MouseWheelBackwardEvent(XQ::Vec2i windowpos, XQ::KeyboardModifier, XEvent& event)
{
	if (event.isStopPropagate()) {
		return;
	}

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

void XManipulatorHandler::slotRenderNodeHasSelected(sptr<XRenderNode> node)
{
	std::cout << "slotRenderNodeHasSelected\n";
	if (!mData->enable) {
		return;
	}
	if (!node) {
		//单击了空白区域
		//此时需要关闭交互模式
		mData->mIntaractType = XTransformGizmoRenderNode::InteractObjectType::none;
		return;
	}
	auto gizmoRenderNode = mData->getGizmoRenderNode();
	auto found_node = gizmoRenderNode->findNodeById(node->getID());
	if (node == found_node) {
		auto type = gizmoRenderNode->getInteractObjectType(found_node);
		mData->mIntaractType = type;
		XLOG_INFO("Manipulator Interact type:{}", XQ_META::enum_to_string(type));
		return;
	}
	else {
		mData->mIntaractType = XTransformGizmoRenderNode::InteractObjectType::none;
	}

	auto	gizmoRenderNode_parent = gizmoRenderNode->getRenderNodeParent();
	auto node_parent = node->getRenderNodeParent();
	auto hasBindnode = gizmoRenderNode->getBindRenderNode();

	if (hasBindnode != node) {
		if (hasBindnode && gizmoRenderNode_parent) {
			gizmoRenderNode_parent->removeChild(gizmoRenderNode);
		}

		node_parent->addChild(gizmoRenderNode);
		gizmoRenderNode->bindRenderNode(node);
	}
}

void XManipulatorHandler::slotManipulatorModeChanged(bool flag)
{
	mData->enable = flag;
	if (flag == false) {
		auto gizmoRenderNode = mData->getGizmoRenderNode();
		if (gizmoRenderNode->getParent()) {
			auto gizmoRenderNode_parent = gizmoRenderNode->getParent()->getParent();
			if (gizmoRenderNode_parent) {
				if (auto gizmoRenderNode_parent2 = gizmoRenderNode_parent->asDerived<XRenderNode>()) {
					gizmoRenderNode_parent2->removeChild(gizmoRenderNode);
				}
			}
		}
	}
}
