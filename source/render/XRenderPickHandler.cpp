#include "XRenderPickHandler.h"
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
	
	PickData lastPickData;
	PickData lastTouctPickData;

	bool extInterfaceEnable =false;
};

XRenderPickHandler::XRenderPickHandler():mData(new Internal)
{
}

XRenderPickHandler::~XRenderPickHandler()
{
}


void XRenderPickHandler::LeftButtonReleaseEvent(XQ::Vec2i windowpos, XQ::KeyboardModifier m, XEvent& event)
{
	if (event.isStopPropagate()) {
		return;
	}

	if (!isRenderActive())
		return;

	mData->mouseType = MouseType::none;

	auto pick_data = mainFboObjectSelected(windowpos);

	getRender()->makeCurrent();
	if (auto node = pick_data.PickNode.lock()) {
		if (auto iterface = node->getExtInterface();iterface && isExtInterfaceEbable()) {
			iterface->LeftButtonReleaseEvent(getRender(), windowpos, m, event, pick_data.FragCoord);
		}
	}
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

void XRenderPickHandler::LeftButtonDoublePressEvent(XQ::Vec2i windowpos, XQ::KeyboardModifier m, XEvent& event)
{
	if (event.isStopPropagate()) {
		return;
	}

	if (!isRenderActive())
		return;

	mData->mouseLstPos = getRender()->window2render(windowpos);

	{
		auto pick_data = overlayFboObjectSelected(windowpos, 0);
		if (auto node = pick_data.PickNode.lock()) {
			if (auto iterface = node->getExtInterface(); iterface && isExtInterfaceEbable()) {
				iterface->LeftButtonDoublePressEvent(getRender(), windowpos, m, event,pick_data.FragCoord);
			}
			else {
				auto object_data = pick_data.SelectData;
				auto objectId = object_data[1];
				auto primitiveId = object_data[2];

				if (auto geomNode = node->asDerived< XGeometryNode>()) {
					if (geomNode->getName() == GLOBALE_XAIS_NODE_NAME) {
						auto normal = geomNode->getInput()->getFaceNormal(primitiveId);
						getRender()->getCamera()->setEyeDir(normal);
					}
				}
			}
		}
	}
}

void XRenderPickHandler::RightButtonDoublePressEvent(XQ::Vec2i, XQ::KeyboardModifier, XEvent& event)
{

}

void XRenderPickHandler::MiddleButtonDoublePressEvent(XQ::Vec2i, XQ::KeyboardModifier, XEvent& event)
{

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

void XRenderPickHandler::LeftButtonPressEvent(XQ::Vec2i windowpos, XQ::KeyboardModifier m, XEvent& event)
{
	if (event.isStopPropagate()) {
		return;
	}

	if (!isRenderActive())
		return;

	mData->mouseLstPos = getRender()->window2render(windowpos);
	mData->mouseType = MouseType::left;

	auto pick_data = mainFboObjectSelected(windowpos);

	getRender()->makeCurrent();
	if (auto node = pick_data.PickNode.lock()) {
		{
			auto object_data = pick_data.SelectData;
			auto objectId = object_data[1];
			{
				if (auto geomNode = node->asDerived< XGeometryNode>()) {
					if (auto lastNode = mData->lastPickData.PickNode.lock()) {
						if (auto geom_node = lastNode->asDerived<XGeometryNode>()) {
							geom_node->Attribute->AttrDrawOutline->setValue(false);
						}
					}
					geomNode->Attribute->AttrDrawOutline->setValue(true);
					//如果原始状态有预选，则需要清除
					{
						if (auto n = mData->lastPickData.PickNode.lock()) {
							if (auto geom = n->asDerived<XGeometryNode>()) {
								geom->State->setFaceState(mData->lastPickData.SelectData[2], PrimitiveState::normal);
							}
						}
					}

					PickMode_union flags;
					flags.value = object_data[3];
					if (flags.bits.face) {
						geomNode->State->setFaceState(object_data[2], PrimitiveState::selected | PrimitiveState::normal);
						auto s = geomNode->State->getFaceState(object_data[2]);
					}
				}
			}
			if (auto iterface = node->getExtInterface(); iterface && isExtInterfaceEbable()) {
				iterface->LeftButtonPressEvent(getRender(), windowpos, m, event, pick_data.FragCoord);
			}
		}
		SigRenderNodeSelected(node, pick_data.SelectData,m);
	}else{
		SigRenderNodeSelected(nullptr, pick_data.SelectData,m);
	}

	mData->lastPickData = pick_data;
	{
		auto pick_data = overlayFboObjectSelected(windowpos,0);
		if(auto node = pick_data.PickNode.lock()){
			if (auto iterface = node->getExtInterface(); iterface && isExtInterfaceEbable()) {
				iterface->LeftButtonPressEvent(getRender(),windowpos, m, event, pick_data.FragCoord);
			}else{
				//do nth
			}
		}else{
		
		}
	}
	getRender()->doneCurrent();
}

void XRenderPickHandler::MouseMoveEvent(XQ::Vec2i windowpos, XQ::KeyboardModifier m, XEvent& event)
{
	if (event.isStopPropagate()) {
		return;
	}

	if (!isRenderActive())
		return;

	mData->mouseLstPos = getRender()->window2render(windowpos);

	auto pick_data = mainFboObjectSelected(windowpos);
	getRender()->makeCurrent();
	if (auto node = pick_data.PickNode.lock()) {
		if (auto iterface = node->getExtInterface(); iterface && isExtInterfaceEbable()) {
			iterface->MouseMoveEvent(getRender(), windowpos, m, event, pick_data.FragCoord);
		}
		auto object_data = pick_data.SelectData;

		//实例化ID 模型ID 图元ID //备用32位
		auto objectId = object_data[1];
		{
			if (auto geomNode = node->asDerived< XGeometryNode>()) {
				//去除预选
				{
					auto lastSelectData = mData->lastTouctPickData.SelectData;
					if (lastSelectData[0] == object_data[0] ||
						lastSelectData[0] == object_data[1] ||
						lastSelectData[0] == object_data[2] ||
						lastSelectData[0] == object_data[3]
						) {
						if (auto n = mData->lastTouctPickData.PickNode.lock()) {
							if (auto geom = n->asDerived<XGeometryNode>()) {
								auto oldState = (PrimitiveState)geom->State->getFaceState(lastSelectData[2]);
								oldState -= PrimitiveState::preselect;
								geom->State->setFaceState(lastSelectData[2], oldState);
							}
						}
					}

				}
				mData->lastTouctPickData.PickNode = node;
				mData->lastTouctPickData.SelectData = object_data;
				PickMode_union flags;
				flags.value = object_data[3];
				if (flags.bits.face) {
					auto oldState = (PrimitiveState)geomNode->State->getFaceState(mData->lastTouctPickData.SelectData[2]);
					oldState |= PrimitiveState::preselect;
					geomNode->State->setFaceState(object_data[2], oldState);
				}
			}
		}
	}
	getRender()->doneCurrent();
}

void XRenderPickHandler::MouseWheelForwardEvent(XQ::Vec2i windowpos, XQ::KeyboardModifier m, XEvent& event)
{
	if (event.isStopPropagate()) {
		return;
	}

	if (!isRenderActive())
		return;

	auto pick_data = mainFboObjectSelected(windowpos);

	if (auto node = pick_data.PickNode.lock()) {
		if (auto iterface = node->getExtInterface(); iterface && isExtInterfaceEbable()) {
			iterface->MouseWheelForwardEvent(getRender(), windowpos, m, event, pick_data.FragCoord);
			//event.stopPropagate();
		}
	}
}

void XRenderPickHandler::MouseWheelBackwardEvent(XQ::Vec2i windowpos, XQ::KeyboardModifier m, XEvent& event)
{
	if (event.isStopPropagate()) {
		return;
	}

	if (!isRenderActive())
		return;

	auto pick_data = mainFboObjectSelected(windowpos);

	if (auto node = pick_data.PickNode.lock()) {
		if (auto iterface = node->getExtInterface(); iterface && isExtInterfaceEbable()) {
			iterface->MouseWheelBackwardEvent(getRender(), windowpos, m, event, pick_data.FragCoord);
			//event.stopPropagate();
		}
	}
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

PickData XRenderPickHandler::mainFboObjectSelected(XQ::Vec2i windowpos)
{
	auto render = getRender();
	auto view_port = render->getConvertViewPort();
	auto mainRenderTarget = render->getMainRenderTarget();
	auto mainRenderFbo = mainRenderTarget->asDerived<XOpenGLFramebufferObject>();
	PickData result;
	render->makeCurrent();
	{
		XQ::Vec4u object_data;
		mainRenderFbo->readPixel(XOpenGLFramebufferObject::Attachment::Color,
			mData->mouseLstPos[0], mData->mouseLstPos[1], 1, 1,
			XOpenGL::TextureExternalFormat::RGBA_Integer, XOpenGL::DataType::unsigned_int, object_data.data, 1);

		XQ::Vec3f pos;
		mainRenderFbo->readPixel(XOpenGLFramebufferObject::Attachment::Depth,
			mData->mouseLstPos[0], mData->mouseLstPos[1], 1, 1,
			XOpenGL::TextureExternalFormat::Depth, XOpenGL::DataType::float_, &pos[2], 1);

		pos[0] = windowpos[0] / (float)view_port[2];
		pos[1] = windowpos[1] / (float)view_port[3];
		result.FragCoord = pos;
		result.SelectData = object_data;
		if (object_data[1] != 0) {
			auto objectId = object_data[1];
			auto node = getRender()->getRenderNode3D(objectId);
			result.PickNode = node;
		}
		//实例化ID 模型ID 图元ID //备用32位
		#if 0
		if (object_data[1] != 0) {
			auto objectId = object_data[1];
			auto node = getRender()->getRenderNode3D(objectId);
			result.PickNode = node;
			if (node) {
				if (auto geomNode = node->asDerived< XGeometryNode>()) {
					if (auto lastNode = mData->lastPickData.PickNode.lock()) {
						if (auto geom_node = lastNode->asDerived<XGeometryNode>()) {
							geom_node->Attribute->AttrDrawOutline->setValue(false);
						}
					}
					geomNode->Attribute->AttrDrawOutline->setValue(true);
					//如果原始状态有预选，则需要清除
					{
						if (auto n = mData->lastPickData.PickNode.lock()) {
							if (auto geom = n->asDerived<XGeometryNode>()) {
								geom->State->setFaceState(mData->lastPickData.SelectData[2], PrimitiveState::normal);
							}
						}
					}
					mData->lastPickData.PickNode = node;
					mData->lastPickData.SelectData = object_data;
					PickMode_union flags;
					flags.value = object_data[3];
					if (flags.bits.face) {
						geomNode->State->setFaceState(object_data[2], PrimitiveState::selected | PrimitiveState::normal);
						auto s = geomNode->State->getFaceState(object_data[2]);
					}
				}

				SigRenderNodeSelected(node, object_data);

				//修改节点的颜色
			}
			else {
				SigRenderNodeSelected(nullptr, object_data);
			}

			std::cout << "objectId:" << objectId << " primitiveId:" << object_data[2] << std::endl;
		}
		#endif
	}
	render->doneCurrent();
	return result;
}

PickData XRenderPickHandler::overlayFboObjectSelected(XQ::Vec2i windowpos, int lay)
{
	PickData result;
	auto render = getRender();
	auto view_port = render->getConvertViewPort();
	auto overlayRenderTarget_0 = render->getOverlayRenderTarget(lay);
	auto overlayRenderTargetFbo_0 = overlayRenderTarget_0->asDerived<XOpenGLFramebufferObject>();

	render->makeCurrent();
	{
		XQ::Vec4u object_data;
		overlayRenderTargetFbo_0->readPixel(XOpenGLFramebufferObject::Attachment::Color,
			mData->mouseLstPos[0], mData->mouseLstPos[1], 1, 1,
			XOpenGL::TextureExternalFormat::RGBA_Integer, XOpenGL::DataType::unsigned_int, object_data.data, 1);
		result.SelectData = object_data;

		XQ::Vec3f pos;
		overlayRenderTargetFbo_0->readPixel(XOpenGLFramebufferObject::Attachment::Depth,
			mData->mouseLstPos[0], mData->mouseLstPos[1], 1, 1,
			XOpenGL::TextureExternalFormat::Depth, XOpenGL::DataType::float_, &pos[2], 1);

		pos[0] = windowpos[0] / (float)view_port[2];
		pos[1] = windowpos[1] / (float)view_port[3];
		result.FragCoord = pos;
		if (object_data[1] != 0) {
			auto objectId = object_data[1];
			auto primitiveId = object_data[2];
			auto node = getRender()->getRenderNode3D(objectId, 0);
			result.PickNode = node;
		}
		//实例化ID 模型ID 图元ID //备用32位
		#if 0
		if (object_data[1] != 0) {
			auto objectId = object_data[1];
			auto primitiveId= object_data[2];
			auto node = getRender()->getRenderNode3D(objectId,0);
			result.PickNode = node;
			if (node) {
				if (auto geomNode = node->asDerived< XGeometryNode>()) {
					if (geomNode->getName() == GLOBALE_XAIS_NODE_NAME) {
						auto normal = geomNode->getInput()->getFaceNormal(primitiveId);
						getRender()->getCamera()->setEyeDir(normal);
					}
				}
			}
		}
		#endif
	}
	render->doneCurrent();
	return result;
}

bool XRenderPickHandler::isExtInterfaceEbable() const
{
	return mData->extInterfaceEnable;
}

void XRenderPickHandler::setExtInterfaceEbable(bool enable)
{
	mData->extInterfaceEnable = enable;
}
