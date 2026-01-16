#pragma once

#include "XRenderApi.h"
#include <dataBase/XDataBaseObject.h>
#include <dataBase/XDataObject.h>
#include <dataBase/XDataAttribute.h>
#include <lib00_utilty/XUtilty.h>

class xcamera;
class XRender;
class  LIB_XRENDER_API XRenderCamera : public XDataObject {
public:
    enum class ProjectionType {
		perspective,
		ortho
    };
protected:
    XRenderCamera();
    ~XRenderCamera();
    void render(sptr< XRender> render);
public:
    void Init() override;

    void setProjectionType(ProjectionType type);
	void rotate(XQ::Vec2i curPoint, XQ::Vec2i lastPoint, float width, float height);
	void translate(XQ::Vec2i curPoint, XQ::Vec2i lastPoint, float width, float height);
    void resetCamera(XQ::BoundBox box);
    void scale(float factor);

    Eigen::Matrix4f projectionMatrix() const;
    Eigen::Matrix4f getViewMatrix() const;
    void setAspect(float aspect);
    std::vector<XQ::Vec3f> getFrustumInWorld() const;
 protected:
    sptr<xcamera> mCamera;
public:
	//csptr<XAttr_Float> AttrFovy;                                    //attr:视野角
	//csptr<XAttr_Float> AttrZnear;                                   //attr:近裁剪面
	//csptr<XAttr_Float> AttrZfar;                                     //attr:远裁剪面
	//csptr<XAttr_Float> AttrAspect;                                  //attr:宽高比
	//csptr<XAttr_Float> AttrHeight;                                 //attr:视野高度(用于正投影)
    //csptr<XAttr_UInt>  AttrProjectionType;                    //attr:投影类型 cameraType,该值不允许修改
};