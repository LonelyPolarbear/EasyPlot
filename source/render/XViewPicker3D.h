#pragma once
#include "XRenderApi.h"
#include < dataBase/XDataBaseObject.h>
#include < dataBase/XVector.h>
#include "lib04_opengl/XOpenGLFramebufferObject.h"
#include <set>

class XGeometryNode;
class xcamera;
class xshader;
class XRender;

class LIB_XRENDER_API XViewPicker3D :public XDataBaseObject {
public:
	struct SelectData {
		uint32_t objectId{0};
		uint32_t primitiveId{0};
	};
protected:
	XViewPicker3D();
	 ~XViewPicker3D();
public:

bool initGL();

std::shared_ptr< XOpenGLFramebufferObject> getFbo(int index);

SelectData XViewPicker3D::getPointSelection(XQ::Vec2i pos);

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