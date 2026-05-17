#include "XRenderPickHandler.h"
#include "XRender.h"
#include "lib00_utilty/XUtilty.h"
#include "lib02_camera/xcamera.h"
#include "XRenderCamera.h"
#include "lib04_opengl/XOpenGLFuntion.h"
#include "lib04_opengl/XOpenGLBuffer.h"
#include "lib04_opengl/XOpenGLFramebufferObject.h"
#include "xrendernode/XGeometryNode.h"

union PickMode_union
{
	struct{
		uint32_t point : 1;
		uint32_t line : 1;
		uint32_t face : 1;
		uint32_t reserved : 29;
	} bits;
	uint32_t value =0;
};

struct XRenderPickHandler::Internal {
	XQ::Vec2i mouseLstPos;
	MouseType mouseType = MouseType::none;
	wptr<XBaseRenderNode> lastPickNode;
	XQ::Vec4u lastSelectData;

	wptr<XBaseRenderNode> lastPressedPickNode;
	XQ::Vec4u lastPressedSelectData;
};

XRenderPickHandler::XRenderPickHandler():mData(new Internal)
{
}

XRenderPickHandler::~XRenderPickHandler()
{
}

void XRenderPickHandler::LeftButtonPressEvent(XQ::Vec2i windowpos, XQ::KeyboardModifier, XEvent& event)
{
	if (event.isStopPropagate()) {
		return;
	}

	if(!isRenderActive())
		return;
	
	mData->mouseLstPos = getRender()->window2render(windowpos);
	mData->mouseType = MouseType::left;
	std::cout << std::dec;
	XQ::print("XRenderPickHandler LeftButtonPressEvent ", windowpos, mData->mouseLstPos);

	auto render = getRender();
	auto data = render->getRenderObjectData();
	auto fbo = data->asDerived<XOpenGLFramebufferObject>();
	render->makeCurrent();
	XQ::Vec4u object_data;
	fbo->readPixel(XOpenGLFramebufferObject::Attachment::Color,
		mData->mouseLstPos[0], mData->mouseLstPos[1], 1, 1,
		XOpenGL::TextureExternalFormat::RGBA_Integer, XOpenGL::DataType::unsigned_int, object_data.data, 1);

	//实例化ID 模型ID 图元ID //备用32位
	if (object_data[1] != 0) {
		auto objectId = object_data[1];
		auto node = getRender()->getRenderNode3D(objectId);
		if (node) {
			if (auto geomNode = node->asDerived< XGeometryNode>()) {
				if (auto lastNode = mData->lastPressedPickNode.lock()) {
					if (auto geom_node = lastNode->asDerived<XGeometryNode>()) {
						geom_node->Attribute->AttrDrawOutline->setValue(false);
					}
				}
				geomNode->Attribute->AttrDrawOutline->setValue(true);
				//如果原始状态有预选，则需要清除
				{
					if (auto n =mData->lastPressedPickNode.lock()) {
						if (auto geom = n->asDerived<XGeometryNode>()) {
							geom->State->setFaceState(mData->lastPressedSelectData[2], PrimitiveState::normal);
						}	
					}
				}
				mData->lastPressedPickNode = node;
				mData->lastPressedSelectData = object_data;
				PickMode_union flags;
				flags.value = object_data[3];
				if (flags.bits.face) {
					geomNode->State->setFaceState(object_data[2],PrimitiveState::selected| PrimitiveState::normal);
					auto s = geomNode->State->getFaceState(object_data[2]);
					int i=0;
				}
			}

			SigRenderNodeSelected(node, object_data);

			//修改节点的颜色
		}
		
		std::cout << "objectId:" << objectId << " primitiveId:" << object_data[2] << std::endl;
	}

	render->doneCurrent();
}

void XRenderPickHandler::LeftButtonReleaseEvent(XQ::Vec2i windowpos, XQ::KeyboardModifier, XEvent& event)
{
	if (event.isStopPropagate()) {
		return;
	}

	if (!isRenderActive())
		return;

	mData->mouseType = MouseType::none;
	XQ::print("XRenderPickHandler LeftButtonReleaseEvent");
}

void XRenderPickHandler::MiddleButtonPressEvent(XQ::Vec2i windowpos, XQ::KeyboardModifier, XEvent& event)
{
	if (event.isStopPropagate()) {
		return;
	}

	if (!isRenderActive())
		return;

	mData->mouseLstPos = getRender()->window2render(windowpos);
	mData->mouseType = MouseType::middle;
}

void XRenderPickHandler::MiddleButtonReleaseEvent(XQ::Vec2i windowpos, XQ::KeyboardModifier, XEvent& event)
{
	if (event.isStopPropagate()) {
		return;
	}

	if (!isRenderActive())
		return;
	mData->mouseType = MouseType::none;
}

void XRenderPickHandler::RightButtonPressEvent(XQ::Vec2i windowpos, XQ::KeyboardModifier, XEvent& event)
{
	if (event.isStopPropagate()) {
		return;
	}

	if (!isRenderActive())
		return;
	mData->mouseType = MouseType::right;
}

void XRenderPickHandler::RightButtonReleaseEvent(XQ::Vec2i windowpos, XQ::KeyboardModifier, XEvent& event)
{
	if (event.isStopPropagate()) {
		return;
	}

	if (!isRenderActive())
		return;
	mData->mouseType = MouseType::none;
}

void XRenderPickHandler::EnterEvent(XEvent& event)
{
	if (event.isStopPropagate()) {
		return;
	}

	if (!isRenderActive())
		return;
}

void XRenderPickHandler::LeaveEvent(XEvent& event)
{
	if (event.isStopPropagate()) {
		return;
	}

	if (!isRenderActive())
		return;
}

void XRenderPickHandler::FoucsInEvent(XEvent& event)
{
	if (event.isStopPropagate()) {
		return;
	}

	if (!isRenderActive())
		return;
}

void XRenderPickHandler::FoucsOutEvent(XEvent& event)
{
	if (event.isStopPropagate()) {
		return;
	}

	if (!isRenderActive())
		return;
}

void XRenderPickHandler::ResizeEvent(XQ::Vec2i size, XEvent& event)
{
	if (event.isStopPropagate()) {
		return;
	}
}

void XRenderPickHandler::KeyPressEvent(XQ::Key, XQ::KeyboardModifier, XEvent& event)
{
	if (event.isStopPropagate()) {
		return;
	}

	if (!isRenderActive())
		return;
}

void XRenderPickHandler::KeyReleaseEvent(XQ::Key, XQ::KeyboardModifier, XEvent& event)
{
	if (event.isStopPropagate()) {
		return;
	}

	if (!isRenderActive())
		return;
}

void XRenderPickHandler::MouseMoveEvent(XQ::Vec2i windowpos, XQ::KeyboardModifier, XEvent& event)
{
	if (event.isStopPropagate()) {
		return;
	}

	if (!isRenderActive())
		return;

	mData->mouseLstPos = getRender()->window2render(windowpos);

	auto render = getRender();
	auto data = render->getRenderObjectData();
	auto fbo = data->asDerived<XOpenGLFramebufferObject>();
	render->makeCurrent();
	XQ::Vec4u object_data;
	fbo->readPixel(XOpenGLFramebufferObject::Attachment::Color,
		mData->mouseLstPos[0], mData->mouseLstPos[1], 1, 1,
		XOpenGL::TextureExternalFormat::RGBA_Integer, XOpenGL::DataType::unsigned_int, object_data.data, 1);

	//实例化ID 模型ID 图元ID //备用32位
	if (object_data[1] != 0) {
		auto objectId = object_data[1];
		auto node = getRender()->getRenderNode3D(objectId);
		if (node) {
			if (auto geomNode = node->asDerived< XGeometryNode>()) {
				//去除预选
				{
					if (mData->lastSelectData[0] == object_data[0] ||
						mData->lastSelectData[0] == object_data[1] ||
						mData->lastSelectData[0] == object_data[2] ||
						mData->lastSelectData[0] == object_data[3]
					) {
						if (auto n = mData->lastPickNode.lock()) {
							if (auto geom = n->asDerived<XGeometryNode>()) {
								auto oldState = (PrimitiveState)geom->State->getFaceState(mData->lastSelectData[2]);
								oldState -= PrimitiveState::preselect;
								geom->State->setFaceState(mData->lastSelectData[2], oldState);
							}
						}
					}
					
				}
				mData->lastPickNode = node;
				mData->lastSelectData = object_data;
				PickMode_union flags;
				flags.value = object_data[3];
				if (flags.bits.face) {
					auto oldState = (PrimitiveState)geomNode->State->getFaceState(mData->lastSelectData[2]);
					oldState |= PrimitiveState::preselect;
					geomNode->State->setFaceState(object_data[2], oldState);
				}
			}
		}
		//std::cout << "objectId:" << objectId << " primitiveId:" << object_data[2] << std::endl;
	}

	render->doneCurrent();
	
}

void XRenderPickHandler::MouseWheelForwardEvent(XQ::Vec2i windowpos, XQ::KeyboardModifier, XEvent& event)
{
	if (event.isStopPropagate()) {
		return;
	}

	if (!isRenderActive())
		return;
}

void XRenderPickHandler::MouseWheelBackwardEvent(XQ::Vec2i windowpos, XQ::KeyboardModifier, XEvent& event)
{
	if (event.isStopPropagate()) {
		return;
	}

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
}

void XRenderPickHandler::slotRenderActiveChanged(bool active)
{
	if (active == false) {
		mData->mouseType = MouseType::none;
	}
}