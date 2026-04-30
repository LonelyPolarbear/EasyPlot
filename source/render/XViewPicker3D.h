#pragma once
#include "renderApi.h"
#include < dataBase/XDataBaseObject.h>
#include < dataBase/XVector.h>
#include <lib04_opengl/XOpenGLFramebufferObject.h>

#include "base/xbaserender/baseRender/XRenderType.h"
#include <set>

class XGeometryNode;
class xcamera;
class xshader;
class XRender;

class RENDER_API XViewPicker3D :public XDataBaseObject {
public:
protected:
	XViewPicker3D();
	 ~XViewPicker3D();
public:

bool initGL();

std::shared_ptr< XOpenGLFramebufferObject> getFbo(int index);

XQ::XSelectData XViewPicker3D::getPointSelection(XQ::Vec2i pos);

 bool renderLayer(int layer);

 int getPeelLayerNum()const;

 void setRender(sptr<XRender> render);

 bool slotRenderSizeChanged(XQ::Vec2i size);

 bool makeCurrent();
 void doneCurrent();
private:
	class Internal;
	std::shared_ptr<Internal> mData;
};