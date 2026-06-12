#pragma once

#include "base/xbaserender/xbaserenderApi.h"
#include "base/xbaserender/baseRender/XBaseRender.h"
#include <dataBase/XDataObject.h>
#include <Eigen/Eigen>
#include <lib00_utilty/XUtilty.h>

class XBASERENDER_API XBaseRenderCamera : public XDataObject {
	REGISTER_CLASS_META_DATA(XBaseRenderCamera, XDataObject);
protected:
	XBaseRenderCamera();
	virtual ~XBaseRenderCamera();

public:
	enum class ProjectionType {
		ortho = 1,
		perspective = 2
	};

	enum class CameraStyle {
		freely = 1,
		upRight = 2
	};

public:
	void Init() override;

	virtual void setProjectionType(ProjectionType type) =0;

	virtual void setEyeDir(const XQ::Vec3f& dir) =0;

	virtual void rotate(XQ::Vec2i curPoint, XQ::Vec2i lastPoint, float width, float height) =0;

	virtual void translate(XQ::Vec2i curPoint, XQ::Vec2i lastPoint, float width, float height) =0;

	virtual void resetCamera(XQ::BoundBox box) =0;

	virtual void scale(float factor) =0;

	virtual ProjectionType getProjectionType() const =0;

	virtual void setNear(double n) = 0;

	virtual void setFar(double n) = 0;

	virtual double getNear() const =0;

	virtual double getFar() const =0;

	virtual Eigen::Matrix4f projectionMatrix() const = 0;

	virtual Eigen::Matrix4f orthoMatrix(float width, float height,float znear,float zfar) const = 0;

	virtual Eigen::Matrix4f perspectiveMatrix(float aspect, float fovy, float znear, float zfar) const = 0;

	virtual Eigen::Matrix4f getViewMatrix() const=0;

	/**
	 * @brief 在相机近平面构造一个坐标系，该坐标系的原点在近平面的中心，X轴 Y轴与相机的X Y保持一致
	 * @return 构造的坐标系在相机坐标系下的姿态
	 */
	virtual Eigen::Matrix4f getNearFrame(sptr<XBaseRender>) const =0;

	/**
	 * @brief 获取视锥近平面在相机坐标系的矩形大小(宽 高)
	 */
	virtual XQ::Vec2f getNearSizeInCamera() const = 0;

	/**
	 * @brief 获取视锥远平面在相机坐标系的矩形大小(宽 高)
	 */
	virtual XQ::Vec2f getFarSizeInCamera() const = 0;

	virtual Eigen::Affine3f& getCameraFrame()=0;

	virtual void setAspect(float aspect) = 0;

	virtual double getAspect() const =0;

	virtual std::vector<XQ::Vec3f> getFrustumInWorld() const =0;

	/**
	 * @breif 对于指定的Z平面，相机切平面与屏幕在水平方向的缩放比例 相机：屏幕宽度，Z是相机坐标系下距离Z平面的距离
	 */
	virtual float scaleFactorH(float zValue, float screenw) =0;

	/**
	 * @breif 对于指定的Z平面，相机切平面与屏幕在竖直方向的缩放比例 相机：屏幕高度
	 */
	virtual float scaleFactorV(float zValue, float screenh) =0;

	/**
	 * @brief 屏幕坐标到NDC坐标，屏幕坐标系为左手坐标系，原点位于屏幕左下角，X轴向右，Y轴向上,Z轴朝向屏幕里，范围[-1,1]
	 * @param input 屏幕坐标系下的一点
	 * @return NDC坐标系下的一点
	 */
	virtual Eigen::Vector3f ComputeDisplayToNdc(Eigen::Vector3f input) const=0;

	/**
	 * @brief NDC坐标到屏幕坐标，屏幕坐标系为左手坐标系，原点位于屏幕左下角，X轴向右，Y轴向上,Z轴朝向屏幕里，范围[-1,1]
	 * @param input NDC坐标系下的一点
	 * @return 屏幕坐标系下的一点
	 */
	virtual Eigen::Vector3f ComputeNdcToDisplay(Eigen::Vector3f input) const=0;


	/**
	 * @brief 世界坐标转屏幕坐标，屏幕坐标系为左手坐标系，原点位于屏幕左下角，X轴向右，Y轴向上,Z轴朝向屏幕里，范围[-1,1]
	 * @param input 世界坐标系下的一点
	 * @return 屏幕坐标系下的一点 范围[0,1]
	 */
	virtual Eigen::Vector3f ComputeWorldToDisplay(Eigen::Vector3f input) const=0;

	/**
	 * @brief 屏幕坐标转世界坐标，屏幕坐标系为左手坐标系，原点位于屏幕左下角，X轴向右，Y轴向上,Z轴朝向屏幕里，范围[0,1]
	 * @param input 屏幕坐标系下的一点
	 * @return 世界坐标系下的一点
	 */
	virtual Eigen::Vector3f ComputeDisplayToWorld(Eigen::Vector3f input) const=0;

	/**
	 * @brief 屏幕坐标转相机坐标系，屏幕坐标系为左手坐标系，原点位于屏幕左下角，X轴向右，Y轴向上,Z轴朝向屏幕里，范围[0,1]
	 * @param input 屏幕坐标系下的一点
	 * @return 相机坐标系下的位置
	 */
	virtual Eigen::Vector3f ComputeDisplayToCamera(Eigen::Vector3f input) const = 0;

	/**
	 * @brief 世界坐标系转相机坐标系
	 * @param input 世界坐标系下的位置
	 * @return 相机坐标系下的位置
	 */
	virtual Eigen::Vector3f ComputeWorldToCamera(Eigen::Vector3f input) const = 0;

	/**
	 * brief 相机坐标系转世界坐标系
	 * @param input 相机坐标系下的位置
	 * @return 世界坐标系下的位置
	 */
	virtual Eigen::Vector3f ComputeCameraToWorld(Eigen::Vector3f input) const = 0;

public:
	csptr<XAttr_Enum<CameraStyle>>  AttrCameraStyle;                    //trackball相机或者fps相机
};