#pragma once

#include "XRenderApi.h"
#include <dataBase/dataobject.h>

class xcamera;
class XRender;
class  LIB_XRENDER_API XRenderCamera : public DataBaseObject {
protected:
    XRenderCamera();
    ~XRenderCamera();
    void render(sptr< XRender> render);
public:
    sptr<xcamera> getInnerCamera() const;
    void Init() override;
 protected:
    sptr<xcamera> mCamera;
};