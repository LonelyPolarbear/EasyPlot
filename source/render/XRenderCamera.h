#pragma once

#include "XRenderApi.h"
#include <dataBase/dataobject.h>

class xcamera;
class XRender;
class  LIB_XRENDER_API XRenderCamera : public DataBaseObject {
protected:
    XRenderCamera();
    ~XRenderCamera();

    void Init() override;
    void render(sptr< XRender> render);
 protected:
    sptr<xcamera> mCamera;
};