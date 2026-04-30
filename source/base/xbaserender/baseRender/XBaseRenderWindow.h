#pragma once
#include "base/xbaserender/xbaserenderApi.h"
#include <dataBase/XDataObject.h>
#include <lib04_opengl/XOpenGLContext.h>
#include <lib01_shader/xshaderManger.h>
class XBaseRender;
class XBaseRenderWindowEventDispatch;
class XBASERENDER_API XBaseRenderWindow : public XDataObject {
	REGISTER_CLASS_META_DATA(XBaseRenderWindow, XDataObject);
protected:
	XBaseRenderWindow();
	virtual ~XBaseRenderWindow();
public:
	virtual void SetWindowId(uint64_t winId) =0;

	virtual void bindSigalSlot() =0;

	virtual void render()=0;

	virtual bool makeCurrent()=0;

	virtual void doneCurrent()=0;

	virtual void swapBuffers()=0;

	virtual sptr<XOpenGLContext> getContext() const =0;

	virtual void slotUpdateSize(XQ::Vec2i size) =0;

	virtual int getWindowWidth() const =0;

	virtual int getWindowHeight() const=0;

	virtual  sptr<XBaseRenderWindowEventDispatch> getEventDispatcher() const=0;

	virtual void addRender(sptr<XBaseRender> ren)=0;

	virtual std::vector<sptr<XBaseRender>> getRenders() const =0;

	virtual sptr<xShaderManger> getShaderManger()const =0;;

};