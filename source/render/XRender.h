#pragma once
#include "XRenderApi.h"
#include "XRenderPort.h"
#include <dataBase/dataobject.h>
#include <lib00_utilty/myUtilty.h>
class XOpenGLRenderWindow;
class XRenderCamera;
class XInteractionEventHandler;
class XShape;
class XGraphicsItem;
class LIB_XRENDER_API XRender :public XRenderPort {
protected:
	XRender();
	~XRender();
	void Init() override;
public:
	void setRenderWindow(sptr< XOpenGLRenderWindow> renderWindow);
	sptr< XOpenGLRenderWindow> getRenderWindow() const;

	void setCamera(sptr<XRenderCamera> camera);
	sptr<XRenderCamera> getCamera() const;

	virtual void render();

	bool makeCurrent();

	void doneCurrent();

	bool setActive(bool isActive);

	bool isActive() const;

	// set the camera navigation handler
	void setCameraNavigationHandler(sptr< XInteractionEventHandler> interactionEventHandler);

	sptr< XInteractionEventHandler> getCameraNavigationHandler();

	bool connectToRenderWindowSignals();

	//
	void addActor3D(sptr<XShape>);

	void addActor2D(sptr<XGraphicsItem>);

	void fitView();

	XQ::Vec2i window2render(const XQ::Vec2i& windowPos);

	XQ::Vec3f render2window(const XQ::Vec2i& renderPos);

	/// <summary>
	/// 获取窗口坐标系下的视口大小(原点左下角)
	/// </summary>
	XQ::Recti getConvertViewPort() const;

	bool isBelongToRender(const XQ::Vec2i& windowPos) const;
protected:

	void updateViewPort();

	void updateUbo();

	sptr< XInteractionEventHandler> getOrCreateMultiModeEventHandler();

	XQ::BoundBox  computeBoundBox();

	struct Internal;
	sptr<Internal> mData;
};
