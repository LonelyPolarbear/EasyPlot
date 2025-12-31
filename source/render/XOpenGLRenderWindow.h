#pragma once
#include "XRenderApi.h"
#include <dataBase/dataobject.h>
#include <lib00_utilty/myUtilty.h>

class XOpenGLContext;
class XRenderWindowInteractor;
class XRender;
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

	sptr<XRenderWindowInteractor> getInteractor() const;

	void addRender(sptr<XRender> ren);
private:
	sptr<XOpenGLContext> m_context;		    //窗口上下文
	XQ::Vec2i m_windowSize;							//窗口尺寸
	sptr<XRenderWindowInteractor> m_interactor;	//渲染窗口交互器
	std::vector<sptr<XRender>> m_renders;
};