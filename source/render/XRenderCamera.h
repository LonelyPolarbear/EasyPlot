#pragma once

#include "renderApi.h"
#include <dataBase/XDataBaseObject.h>
#include <dataBase/XDataObject.h>
#include <dataBase/XDataAttribute.h>
#include <lib00_utilty/XUtilty.h>

class XBaseCmaera;
class XRender;
class  RENDER_API XRenderCamera : public XDataObject {
public:
    enum class ProjectionType {
        ortho=1,
		perspective=2
    };

    enum class CameraStyle {
        freely=1,
        upRight=2
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
    ProjectionType getProjectionType() const;
	double getNear() const;
	double getFar() const;

    Eigen::Matrix4f projectionMatrix() const;
    Eigen::Matrix4f getViewMatrix() const;
    void setAspect(float aspect);
    std::vector<XQ::Vec3f> getFrustumInWorld() const;
 protected:
    sptr<XBaseCmaera> mCamera;
public:
    csptr<XAttr_Enum<CameraStyle>>  AttrCameraStyle;                    //trackball相机或者fps相机
 };