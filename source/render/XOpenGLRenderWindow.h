#pragma once
#include "renderApi.h"
#include <dataBase/XDataBaseObject.h>
#include <dataBase/XDataObject.h>
#include <dataBase/XDataList.h>
#include <lib00_utilty/XUtilty.h>
#include <dataBase/XVector.h>
#include <lib01_shader/xshaderManger.h>
#include "XRender.h"

class XOpenGLContext;
class XOpenGLBuffer;
class XRenderWindowEventDispatch;
//class XRender;

class RENDER_API RenderWindowUbo :public XDataBaseObject {
protected:
	RenderWindowUbo();
	~RenderWindowUbo();

public:
	void create();

	void writeVS(const Eigen::Matrix4f& view, const Eigen::Matrix4f& projection);

	void writeVS2d(const Eigen::Matrix4f& virtualWorldFrame, const Eigen::Matrix4f& sceneFrameInVirtualWorld);

	void writeFS(const XQ::Vec2f& viewport, const XQ::Vec2f& mousePos) ;

	void writeGS(const Eigen::Vector2f& viewport, float sceneScale);

	void writeCamera(int type,double near,double far);
protected:
	std::shared_ptr<XOpenGLBuffer> uniformBufferVs;						//用于传递变换矩阵的缓冲区 绑定点1
	std::shared_ptr<XOpenGLBuffer> uniformBufferVs2d;					//用于传递变换矩阵的缓冲区 绑定点4
	std::shared_ptr<XOpenGLBuffer> uniformBufferFs;						//绑定点2 视口大小和鼠标位置
	std::shared_ptr<XOpenGLBuffer> uniformBufferGs;						//绑定点3
	std::shared_ptr<XOpenGLBuffer> uniformBufferCamera;				//绑定点5,记录相机信息
	bool init_done = false;
};


class RENDER_API XOpenGLRenderWindow :public XDataObject {
	REGISTER_CLASS_META_DATA(XOpenGLRenderWindow, XDataObject);
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
	std::vector<sptr<XRender>> getRenders() const;

	sptr<xShaderManger> getShaderManger()const;

	sptr< RenderWindowUbo> getRenderWindowUbo() const;
private:
	sptr<xShaderManger> m_shaderManger;								//着色器管理器
	sptr<XOpenGLContext> m_context;										//窗口上下文
	XQ::Vec2i m_windowSize;														//窗口尺寸
	sptr<XRenderWindowEventDispatch> m_eventDispatch;	//渲染窗口交互器
	//std::vector<sptr<XRender>> m_renders;
	sptr< RenderWindowUbo> m_renderWindowUbo;

	csptr<XDataListT<XRender> > m_renderList;
};