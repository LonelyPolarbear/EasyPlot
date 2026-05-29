#pragma once
#include "renderApi.h"
#include <base/xbaserender/baseRender/XRenderType.h>
#include <base/xbaserender/baseRender/XBaseRender.h>
#include <base/xbaserender/baseRender/XBaseRenderWindow.h>
#include <base/xbaserender/baseRender/XBaseRenderCamera.h>
#include <base/xbaserender/baseRender/XBaseInteractionEventHandler.h>
#include <dataBase/XDataObject.h>
#include <dataBase/XDataList.h>
#include <dataBase/XDataAttribute.h>
#include <lib00_utilty/XUtilty.h>
#include "xsignal/XSignal.h"
class XBaseRenderWindow;
class XRenderCamera;
class XInteractionEventHandler;
class XGeometryNode;
class XGraphicsItem;
class XRenderNode;
class XGroupRenderNode3d;
class XCustomGroupRenderNode3d;

class RENDER_API XRender :public XBaseRender {
	REGISTER_CLASS_META_DATA(XRender, XBaseRender);
protected:
	XRender();
	~XRender();
public:
	void Init() override;
	void setRenderWindow(sptr< XBaseRenderWindow> renderWindow) override;
	sptr< XBaseRenderWindow> getRenderWindow() const override;

	void setCamera(sptr<XBaseRenderCamera> camera) override;

	sptr<XBaseRenderCamera> getCamera() const override;

	sptr<XBaseRenderCamera> getSceenCamera() const override;

	sptr<XDataBaseObject> getMainRenderTarget()  override;

	sptr<XDataBaseObject> getOverlayRenderTarget(int lay)  override;

	void render() override;

	void renderGBuffer() override;

	void renderScreen3dNode2Buffer();

	void renderToScreen() override;

	void renderScreen3dNodeBuffer2Screen();

	bool makeCurrent() override;

	void doneCurrent() override;

	bool setActive(bool isActive) override;

	bool isActive() const override;

	// set the camera navigation handler
	void setCameraNavigationHandler(sptr< XInteractionEventHandler> interactionEventHandler);

	void setPickHandler(sptr< XInteractionEventHandler> pickEventHandler);

	void setManipulatorHandler(sptr< XBaseInteractionEventHandler> manipulatorHandler) override;

	sptr< XInteractionEventHandler> getCameraNavigationHandler();

	sptr< XInteractionEventHandler> getPickHandler();

	sptr< XBaseInteractionEventHandler> getManipulatorHandler() override;

	bool connectToRenderWindowSignals() override;

	void addRenderNode3D(sptr<XBaseRenderNode>) override;

	void addScreenRenderNode3D(sptr<XBaseRenderNode>) override;

	void addRenderNode2D(sptr<XGraphicsItem>);

	void fitView() override;

	XQ::Vec2i window2render(const XQ::Vec2i& windowPos) override;

	XQ::Vec3f render2window(const XQ::Vec2i& renderPos) override;

	/**
	 * @brief 获取窗口坐标系下的视口大小(原点左下角)
	 */
	XQ::Recti getConvertViewPort() const override;

	bool isBelongToRender(const XQ::Vec2i& windowPos) const override;

	void setInteractMode(XQ::InteractMode mode) override;

	XQ::InteractMode getInteractMode() const override;

	sptr<XBaseRenderNode> getRenderNode3D(int id) override;

	sptr<XBaseRenderNode> getRenderNode3D(int id,int lay);

	sptr<XBaseDrawManger> getDrawManger() override;

	/**
	 * @brief 遍历接口，增加一个坐标轴节点
	 */
	sptr<XBaseRenderNode> AddAxisNode();

	/**
	 * @brief 遍历接口，增加一个立方体节点
	 */
	sptr<XBaseRenderNode> AddCubeTestNode();

	/**
	 * @brief 遍历接口，增加无限平面网格节点
	 */
	sptr<XBaseRenderNode> AddInfinitePlaneNode();
public:
	XSIGNAL(void(int id, void*/*data*/)) SigUserEvent;																		//用户自定义事件
	XSIGNAL(void(XQ::PreDefineEvent, void*/*data*/)) SigPredefineEvent;										//预定义业务逻辑事件
protected:
	//属性
	csptr<XAttr_Bool> AttrActive;
	csptr<XAttr_Bool> AttrSmaa;							//是否启用smaa
	csptr<XAttr_Bool> AttrPostProcess;					//是否启用屏幕后处理，如果不启用直接写入屏幕默认帧缓冲
	
	csptr<XCustomGroupRenderNode3d> mMainFboNodes;
	csptr<XGroupRenderNode3d> mScreenNodes;						//特指哪些屏幕固定位置，不允许缩放，但支持旋转的节点
	//csptr<XGeometryNode> mAxisNode;									//坐标轴	
protected:
	void updateUbo();

	sptr< XInteractionEventHandler> getOrCreateMultiModeEventHandler();

	XQ::BoundBox  computeBoundBox();

	struct Internal;
	sptr<Internal> mData;
};
