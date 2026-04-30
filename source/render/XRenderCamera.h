#pragma once

#include "renderApi.h"
#include <dataBase/XDataBaseObject.h>
#include <dataBase/XDataObject.h>
#include <dataBase/XDataAttribute.h>
#include <lib00_utilty/XUtilty.h>
#include "base/xbaserender/baseRender/XBaseRenderCamera.h"
class XBaseCmaera;
class XRender;
class  RENDER_API XRenderCamera : public XBaseRenderCamera {
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

	float scaleFactorH(float zValue, float screenw) override;

	float scaleFactorV(float zValue, float screenh) override;

	Eigen::Vector3f ComputeDisplayToNdc(Eigen::Vector3f input) const override;

	Eigen::Vector3f ComputeNdcToDisplay(Eigen::Vector3f input) const override;

	Eigen::Vector3f ComputeWorldToDisplay(Eigen::Vector3f input) const override;

	Eigen::Vector3f ComputeDisplayToWorld(Eigen::Vector3f input) const override;

	Eigen::Vector3f ComputeDisplayToCamera(Eigen::Vector3f input) const override;

	Eigen::Vector3f ComputeWorldToCamera(Eigen::Vector3f input) const override;

	Eigen::Vector3f ComputeCameraToWorld(Eigen::Vector3f input) const override;

 protected:
    sptr<XBaseCmaera> mCamera;
public:
  
 };