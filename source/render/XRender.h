#pragma once
#include "XRenderApi.h"
#include "XRenderPort.h"
#include "XRenderType.h"
#include <dataBase/XDataObject.h>
#include <dataBase/XDataAttribute.h>
#include <lib00_utilty/XUtilty.h>
#include "xsignal/XSignal.h"
class XOpenGLRenderWindow;
class XRenderCamera;
class XInteractionEventHandler;
class XGeometryNode;
class XGraphicsItem;
class XRenderNode;
class LIB_XRENDER_API XRender :public XRenderPort {
protected:
	XRender();
	~XRender();
public:
	void Init() override;
	void setRenderWindow(sptr< XOpenGLRenderWindow> renderWindow);
	sptr< XOpenGLRenderWindow> getRenderWindow() const;

	void setCamera(sptr<XRenderCamera> camera);
	sptr<XRenderCamera> getCamera() const;

	virtual void render(bool isNormal = true);

	bool makeCurrent();

	void doneCurrent();

	bool setActive(bool isActive);

	bool isActive() const;

	// set the camera navigation handler
	void setCameraNavigationHandler(sptr< XInteractionEventHandler> interactionEventHandler);

	void setPickHandler(sptr< XInteractionEventHandler> pickEventHandler);

	void setManipulatorHandler(sptr< XInteractionEventHandler> manipulatorHandler);

	sptr< XInteractionEventHandler> getCameraNavigationHandler();

	sptr< XInteractionEventHandler> getPickHandler();

	sptr< XInteractionEventHandler> getManipulatorHandler();

	bool connectToRenderWindowSignals();

	void addRenderNode3D(sptr<XRenderNode>);

	void addRenderNode2D(sptr<XGraphicsItem>);

	void fitView();

	XQ::Vec2i window2render(const XQ::Vec2i& windowPos);

	XQ::Vec3f render2window(const XQ::Vec2i& renderPos);

	/// <summary>
	/// 获取窗口坐标系下的视口大小(原点左下角)
	/// </summary>
	XQ::Recti getConvertViewPort() const;

	bool isBelongToRender(const XQ::Vec2i& windowPos) const;

	void addInfinitePlane(Eigen::Matrix4f planeFrame);

	void setInteractMode(XQ::InteractMode mode);

	XQ::InteractMode getInteractMode() const;
public:
	XSIGNAL(void(int id, void*/*data*/)) SigUserEvent;																		//用户自定义事件
	XSIGNAL(void(XQ::PreDefineEvent, void*/*data*/)) SigPredefineEvent;										//预定义业务逻辑事件
protected:
	//属性
	csptr<XAttr_Bool> AttrActive;
protected:

	void updateViewPort(bool isNormal);

	void updateUbo();

	sptr< XInteractionEventHandler> getOrCreateMultiModeEventHandler();

	XQ::BoundBox  computeBoundBox();

	struct Internal;
	sptr<Internal> mData;
};
