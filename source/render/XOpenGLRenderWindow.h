#pragma once
#include "XRenderApi.h"
#include <dataBase/dataobject.h>
#include <lib00_utilty/XUtilty.h>
#include <dataBase/XVector.h>
#include <lib01_shader/xshaderManger.h>

class XOpenGLContext;
class XOpenGLBuffer;
class XRenderWindowEventDispatch;
class XRender;

class LIB_XRENDER_API RenderWindowUbo :public DataBaseObject {
protected:
	RenderWindowUbo();
	~RenderWindowUbo();

public:
	void create();

	void writeVS(const Eigen::Matrix4f& view, const Eigen::Matrix4f& projection);

	void writeVS2d(const Eigen::Matrix4f& virtualWorldFrame, const Eigen::Matrix4f& sceneFrameInVirtualWorld);

	void writeFS(const XQ::Vec2f& viewport, const XQ::Vec2f& mousePos) ;

	void writeGS(const Eigen::Vector2f& viewport, float sceneScale);
protected:
	std::shared_ptr<XOpenGLBuffer> uniformBufferVs;               //用于传递变换矩阵的缓冲区
	std::shared_ptr<XOpenGLBuffer> uniformBufferVs2d;           //用于传递变换矩阵的缓冲区
	std::shared_ptr<XOpenGLBuffer> uniformBufferFs;               //
	std::shared_ptr<XOpenGLBuffer> uniformBufferGs;               //
	bool init_done = false;
};


class LIB_XRENDER_API XOpenGLRenderWindow :public DataBaseObject {
protected:
	XOpenGLRenderWindow();
	virtual ~XOpenGLRenderWindow();
public:
	void SetWindowId(uint64_t winId);

	void Init() override;

	virtual void bindSigalSlot();

	void render();

	bool makeCurrent();

	void doneCurrent();

	void swapBuffers();

	sptr<XOpenGLContext> getContext() const;

	void slotUpdateSize(XQ::Vec2i size);

	int getWindowWidth() const;

	int getWindowHeight() const;

	sptr<XRenderWindowEventDispatch> getEventDispatcher() const;

	void addRender(sptr<XRender> ren);

	sptr<xShaderManger> getShaderManger()const;

	sptr< RenderWindowUbo> getRenderWindowUbo() const;
private:
	sptr<xShaderManger> m_shaderManger;								//着色器管理器
	sptr<XOpenGLContext> m_context;										//窗口上下文
	XQ::Vec2i m_windowSize;														//窗口尺寸
	sptr<XRenderWindowEventDispatch> m_eventDispatch;	//渲染窗口交互器
	std::vector<sptr<XRender>> m_renders;
	sptr< RenderWindowUbo> m_renderWindowUbo;
};