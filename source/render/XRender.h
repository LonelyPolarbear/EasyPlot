#pragma once
#include "XRenderApi.h"
#include <dataBase/dataobject.h>
#include <lib00_utilty/myUtilty.h>
class XOpenGLRenderWindow;
class XRenderCamera;
class LIB_XRENDER_API XRender :public DataBaseObject {
protected:
	XRender();
	~XRender();
public:
	void setRenderWindow(sptr< XOpenGLRenderWindow> renderWindow);
	sptr< XOpenGLRenderWindow> getRenderWindow();

	void setCamera(sptr<XRenderCamera> camera);
	sptr<XRenderCamera> getCamera() const;

	virtual void render();

	bool makeCurrent();

	void doneCurrent();
protected:
	wptr< XOpenGLRenderWindow> m_renderWindow;
	sptr<XRenderCamera> m_camera;
};
