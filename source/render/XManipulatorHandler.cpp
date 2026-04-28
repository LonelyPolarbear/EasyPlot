#include "XManipulatorHandler.h"
#include "XRender.h"
#include "lib00_utilty/XUtilty.h"
#include "lib02_camera/xcamera.h"
#include "XRenderCamera.h"
#include <xsignal/XSignal.h>
#include <xrendernode/renderNode3d/XTransformGizmoRenderNode.h>
#include <xlog/XLogger.h>

#include <Eigen/Eigen>

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

	Eigen::Affine3f objectMat = Eigen::Affine3f::Identity();
	Eigen::Affine3f parentMat = Eigen::Affine3f::Identity();
	Eigen::Affine3f totalMat = Eigen::Affine3f::Identity();

	if(mData->mIntaractType == XTransformGizmoRenderNode::InteractObjectType::none)
		return;
	auto  gizmoRenderNode = mData->getGizmoRenderNode();
	gizmoRenderNode->getChainTransform(totalMat);
	objectMat = gizmoRenderNode->getTransform();
	//Eigen::Affine3f totalMat = parentMat * objectMat;
	auto curpos = getRender()->window2render(windowpos);
	auto viewport = getRender()->getConvertViewPort();


	if (mData->mIntaractType == XTransformGizmoRenderNode::InteractObjectType::translate_x ||
		mData->mIntaractType == XTransformGizmoRenderNode::InteractObjectType::translate_y ||
		mData->mIntaractType == XTransformGizmoRenderNode::InteractObjectType::translate_z
	) 
	{
		//获取当前的向量在屏幕控件的向量投影
		auto local_len = 1.f;
		Eigen::Vector3f local_dir = Eigen::Vector3f::UnitX();
		if (mData->mIntaractType == XTransformGizmoRenderNode::InteractObjectType::translate_y) {
			local_dir = Eigen::Vector3f::UnitY();
		}
		else if (mData->mIntaractType == XTransformGizmoRenderNode::InteractObjectType::translate_z) {
			local_dir = Eigen::Vector3f::UnitZ();
		}

		Eigen::Vector3f loacl_first_point = Eigen::Vector3f(0, 0, 0);
		Eigen::Vector3f loacl_second_point = loacl_first_point + local_dir * local_len;


		Eigen::Vector3f word_first_point = totalMat * loacl_first_point;
		Eigen::Vector3f word_second_point = totalMat * loacl_second_point;

		auto screen_normal_pos0 = getRender()->getCamera()->ComputeWorldToDisplay(word_first_point);
		auto screen_normal_pos1 = getRender()->getCamera()->ComputeWorldToDisplay(word_second_point);

		Eigen::Vector2f screen_pos0 = Eigen::Vector2f(screen_normal_pos0[0], screen_normal_pos0[1]);
		Eigen::Vector2f screen_pos1 = Eigen::Vector2f(screen_normal_pos1[0], screen_normal_pos1[1]);

		//转换到屏幕坐标系
		screen_pos0[0] *= viewport[2];
		screen_pos0[1] *= viewport[3];

		screen_pos1[0] *= viewport[2];
		screen_pos1[1] *= viewport[3];

		//
		Eigen::Vector2f screen_dir = screen_pos1 - screen_pos0;
		auto len = screen_dir.norm();
		screen_dir.normalize();

		//计算缩放比例
		float scale = local_len / (float)len;		//世界：屏幕

		Eigen::Vector2f delta = Eigen::Vector2f(curpos[0] - mData->mouseLstPos[0], curpos[1] - mData->mouseLstPos[1]);
		//计算移动在屏幕方向的投影
		float ss = delta.dot(screen_dir);

		//世界坐标系期望的长度
		float real_stride = ss * scale;

		//计算移动的世界坐标系长度
		objectMat.translate(local_dir * real_stride);

		gizmoRenderNode->setTransform(objectMat);
		gizmoRenderNode->notifySigMatrixChanged();
		//更新位置
		mData->mouseLstPos = getRender()->window2render(windowpos);
		
	}
	if (mData->mIntaractType == XTransformGizmoRenderNode::InteractObjectType::rotate_x ||
		mData->mIntaractType == XTransformGizmoRenderNode::InteractObjectType::rotate_y ||
		mData->mIntaractType == XTransformGizmoRenderNode::InteractObjectType::rotate_z
		) {
		//旋转交互

		//首先获取旋转轴在屏幕空间的投影‘
		auto local_len = 1.f;
		Eigen::Vector3f local_dir = Eigen::Vector3f::UnitX();
		if (mData->mIntaractType == XTransformGizmoRenderNode::InteractObjectType::rotate_y) {
			local_dir = Eigen::Vector3f::UnitY();
		}
		else if (mData->mIntaractType == XTransformGizmoRenderNode::InteractObjectType::rotate_z) {
			local_dir = Eigen::Vector3f::UnitZ();
		}

		Eigen::Vector3f loacl_first_point = Eigen::Vector3f(0, 0, 0);
		Eigen::Vector3f loacl_second_point = loacl_first_point + local_dir * local_len;


		Eigen::Vector3f word_first_point = totalMat * loacl_first_point;
		Eigen::Vector3f word_second_point = totalMat * loacl_second_point;

		auto screen_normal_pos0 = getRender()->getCamera()->ComputeWorldToDisplay(word_first_point);
		auto screen_normal_pos1 = getRender()->getCamera()->ComputeWorldToDisplay(word_second_point);

		Eigen::Vector2f screen_pos0 = Eigen::Vector2f(screen_normal_pos0[0], screen_normal_pos0[1]);
		Eigen::Vector2f screen_pos1 = Eigen::Vector2f(screen_normal_pos1[0], screen_normal_pos1[1]);

		//转换到屏幕坐标系
		screen_pos0[0] *= viewport[2];
		screen_pos0[1] *= viewport[3];

		screen_pos1[0] *= viewport[2];
		screen_pos1[1] *= viewport[3];

		//
		Eigen::Vector2f screen_dir = screen_pos1 - screen_pos0;
		auto len = screen_dir.norm();
		screen_dir.normalize();
		Eigen::Vector2f rotate_axis = screen_dir;
		//
		Eigen::Vector2f delta = Eigen::Vector2f(curpos[0] - mData->mouseLstPos[0], curpos[1] - mData->mouseLstPos[1]);
		
		//计算垂直于屏幕方向的投影
		float ss = delta.dot(screen_dir);
		Eigen::Vector2f dir = delta - ss * screen_dir;

		float sign = 1;
		sign =std::signbit(Eigen::Vector3f( dir[0],dir[1],0).cross(Eigen::Vector3f(rotate_axis[0], rotate_axis[1], 0))[2]) ? 1 : -1;
		auto angle = dir.norm()*2;
		if (abs(angle) > 0.001f) {
			gizmoRenderNode->rotate(sign*angle,XQ::Vec3f(local_dir[0],local_dir[1],local_dir[2]));
		}
		gizmoRenderNode->notifySigMatrixChanged();
		//更新位置
		mData->mouseLstPos = getRender()->window2render(windowpos);
	}

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

	auto	gizmoRenderNode_parent = gizmoRenderNode->getRenderNodeParent();			//操作柄节点的父节点
	auto node_parent = node->getRenderNodeParent();														//当前选中节点的父节点
	auto hasBindnode = gizmoRenderNode->getBindRenderNode();										//操作柄节点关联的渲染节点

	if (hasBindnode != node) {
		if (hasBindnode && gizmoRenderNode_parent) {
			gizmoRenderNode_parent->removeChild(gizmoRenderNode);								//
		}

		node_parent->addChild(gizmoRenderNode);																//将操作柄节点添加到渲染节点的父节点下
		gizmoRenderNode->bindRenderNode(node);																//重新关联新的渲染节点
	}
}

void XManipulatorHandler::slotManipulatorModeChanged(bool flag)
{
	mData->enable = flag;
	if (flag == false) {
		auto gizmoRenderNode = mData->getGizmoRenderNode();
		gizmoRenderNode->bindRenderNode(nullptr);
		auto gizmoRenderNode_parent = gizmoRenderNode->getRenderNodeParent();
		if (gizmoRenderNode_parent) {
			gizmoRenderNode_parent->removeChild(gizmoRenderNode);
		}
	}
}
