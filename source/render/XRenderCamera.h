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
	void setEyeDir(const XQ::Vec3f& dir) override;
	void rotate(XQ::Vec2i curPoint, XQ::Vec2i lastPoint, float width, float height) override;
	void translate(XQ::Vec2i curPoint, XQ::Vec2i lastPoint, float width, float height) override;
    void resetCamera(XQ::BoundBox box) override;
    void scale(float factor) override;
    ProjectionType getProjectionType() const;

	void setNear(double n) override;
	void setFar(double f) override;
	double getNear() const;
	double getFar() const;

    Eigen::Matrix4f projectionMatrix() const;
    Eigen::Matrix4f getViewMatrix() const;

	Eigen::Affine3f& getCameraFrame() override;

	Eigen::Matrix4f orthoMatrix(float width,float height, float znear, float zfar) const override;

	Eigen::Matrix4f perspectiveMatrix(float aspect, float fovy, float znear, float zfar) const override;

	Eigen::Matrix4f getNearFrame(sptr<XBaseRender>) const override;

	XQ::Vec2f getNearSizeInCamera() const override;

	XQ::Vec2f getFarSizeInCamera() const override;

	void setAspect(float aspect);
	double getAspect() const override;

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

	/**
	 * @brief 相机的四条射线与平面的交点，计算的交点是在m坐标系下的交点
	 */
	std::array< XQ::Vec3f, 4> getFrustumIntersections(const Eigen::Matrix4f& m, int type=2/*0 1 2 表示 X Y Z平面*/);

 protected:
    sptr<XBaseCmaera> mCamera;
public:
  
 };