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

	/**
	 * @brief 屏幕坐标到NDC坐标，屏幕坐标系为左手坐标系，原点位于屏幕左下角，X轴向右，Y轴向上,Z轴朝向屏幕里，范围[-1,1]
	 * @param input 屏幕坐标系下的一点
	 * @return NDC坐标系下的一点
	 */
	Eigen::Vector3f ComputeDisplayToNdc(Eigen::Vector3f input) const;

	/**
	 * @brief NDC坐标到屏幕坐标，屏幕坐标系为左手坐标系，原点位于屏幕左下角，X轴向右，Y轴向上,Z轴朝向屏幕里，范围[-1,1]
	 * @param input NDC坐标系下的一点
	 * @return 屏幕坐标系下的一点
	 */
	Eigen::Vector3f ComputeNdcToDisplay(Eigen::Vector3f input) const;


	/**
	 * @brief 世界坐标转屏幕坐标，屏幕坐标系为左手坐标系，原点位于屏幕左下角，X轴向右，Y轴向上,Z轴朝向屏幕里，范围[-1,1]
	 * @param input 世界坐标系下的一点
	 * @return 屏幕坐标系下的一点 范围[0,1]
	 */
	Eigen::Vector3f ComputeWorldToDisplay(Eigen::Vector3f input) const;

	/**
	 * @brief 屏幕坐标转世界坐标，屏幕坐标系为左手坐标系，原点位于屏幕左下角，X轴向右，Y轴向上,Z轴朝向屏幕里，范围[0,1]
	 * @param input 屏幕坐标系下的一点
	 * @return 世界坐标系下的一点
	 */
	Eigen::Vector3f ComputeDisplayToWorld(Eigen::Vector3f input) const;

	/**
	 * @brief 屏幕坐标转相机坐标系，屏幕坐标系为左手坐标系，原点位于屏幕左下角，X轴向右，Y轴向上,Z轴朝向屏幕里，范围[0,1]
	 * @param input 屏幕坐标系下的一点
	 * @return 相机坐标系下的位置
	 */
	Eigen::Vector3f ComputeDisplayToCamera(Eigen::Vector3f input) const;

	/**
	 * @brief 世界坐标系转相机坐标系
	 * @param input 世界坐标系下的位置
	 * @return 相机坐标系下的位置
	 */
	Eigen::Vector3f ComputeWorldToCamera(Eigen::Vector3f input) const;

	/**
	 * brief 相机坐标系转世界坐标系
	 * @param input 相机坐标系下的位置
	 * @return 世界坐标系下的位置
	 */
	Eigen::Vector3f ComputeCameraToWorld(Eigen::Vector3f input) const;

 protected:
    sptr<XBaseCmaera> mCamera;
public:
    csptr<XAttr_Enum<CameraStyle>>  AttrCameraStyle;                    //trackball相机或者fps相机
 };