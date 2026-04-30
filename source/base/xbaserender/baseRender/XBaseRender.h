#pragma once
#include "base/xbaserender/xbaserenderApi.h"
#include "base/xbaserender/baseRender/XRenderType.h"
#include "base/xbaserender/baseRender/XRenderPort.h"
#include <dataBase/XDataObject.h>

/**
 * @brief The base class for rendering.
 */
class XBaseRenderNode;
class XBaseRenderWindow;
class XBaseRenderCamera;
class XBaseInteractionEventHandler;
class XBASERENDER_API XBaseRender : public XRenderPort {
	REGISTER_CLASS_META_DATA(XBaseRender, XDataObject);
protected:
	XBaseRender();
	virtual ~XBaseRender();
public:
	/**
	 * @brief 设置渲染窗口。
	 */
	virtual void setRenderWindow(sptr< XBaseRenderWindow> renderWindow) =0;

	/**
	 * @brief 获取渲染窗口。
	 */
	virtual sptr< XBaseRenderWindow> getRenderWindow() const =0;

	/**
	 * @brief 设置渲染相机。
	 */
	virtual void setCamera(sptr<XBaseRenderCamera> camera) =0;

	/**
	 * @brief 获取渲染相机。
	 */
	virtual sptr<XBaseRenderCamera> getCamera() const =0;

	/**
	 * @brief 渲染
	 * @param isNormal true表示正常渲染，false表示拾取渲染
	 */
	virtual void render(bool isNormal = true) =0;

	/**
	 * @brief 切换渲染上下文。
	 * @return true表示成功切换，false表示失败。
	 */
	virtual bool makeCurrent() =0;

	/**
	 * @brief 结束渲染上下文。
	 */
	virtual void doneCurrent() =0;

	/**
	 * @brief 设置是否激活。
	 * @param isActive true表示激活，false表示不激活。
	 */
	virtual bool setActive(bool isActive) =0;

	/**
	 * @brief 获取是否激活。
	 * @return true表示激活，false表示不激活。
	 */
	virtual bool isActive() const=0;

	/**
	 * @brief 设置交互处理器。
	 * @param manipulatorHandler 交互处理器。
	 */
	virtual void setManipulatorHandler(sptr< XBaseInteractionEventHandler> manipulatorHandler) =0;

	/**
	 * @brief 获取交互处理器。
	 */
	virtual sptr< XBaseInteractionEventHandler> getManipulatorHandler() =0;

	/**
	 * @brief 连接渲染窗口信号。当XBaseRenderWindow添加render时候，调用
	 */
	virtual bool connectToRenderWindowSignals() =0;

	/**
	 * @brief 添加渲染节点
	 */
	virtual void addRenderNode3D(sptr<XBaseRenderNode>) =0;

	/**
	 * @brie fitView
	 */
	virtual void fitView() =0;

	/**
	 * @brief 窗口坐标系转换到渲染坐标系
	 * @param windowPos 窗口坐标系下的位置(原点在左下角)
	 * @return 转换后的渲染坐标系下的位置(原点在左下角)
	 */
	virtual XQ::Vec2i window2render(const XQ::Vec2i& windowPos) =0;

	/**
	 * @brief 渲染坐标系转换到窗口坐标系
	 * @param renderPos 渲染坐标系下的位置(原点在左下角)
	 * @return 转换后的窗口坐标系下的位置(原点在左下角)
	 */
	virtual XQ::Vec3f render2window(const XQ::Vec2i& renderPos)=0;

	/**
	 * @brief 获取窗口坐标系下的视口大小(原点左下角)
	 */
	virtual XQ::Recti getConvertViewPort() const =0;

	/**
	 * @brief 已知窗口坐标系下的位置，判断是否在渲染窗口内
	 */
	virtual bool isBelongToRender(const XQ::Vec2i& windowPos) const =0;

	virtual void setInteractMode(XQ::InteractMode mode) =0;

	virtual XQ::InteractMode getInteractMode() const =0;

	virtual sptr<XBaseRenderNode> getRenderNode3D(int id) =0;
};