#pragma once

#include "XCameraApi.h"
#include <Eigen/Eigen>
#include "XBaseCamera.h"
#include <lib00_utilty/XUtilty.h>
#include <dataBase/XVector.h>
class lib02_CAMERA_API XTrackBallCamera:public XBaseCmaera
{
protected:
	XTrackBallCamera();

	~XTrackBallCamera();

	Eigen::Matrix4f projectionMatrix() const;

	void scale(float sacleFactor) override;

	void transformFreely(Eigen::Vector2f curPoint, Eigen::Vector2f lastPoint, float width, float height, bool isRotate, bool isYUP = false) override;

	void transformUpright(Eigen::Vector2f curPoint, Eigen::Vector2f lastPoint, float width, float height, bool isRotate, bool isYUP = false)override;

	void setEyeDir(const XQ::Vec3f &dir) override;

	void resetCamera(const double boundBox[6]) override;

public:
	Eigen::Affine3f getCameraFrame() const override;

	Eigen::Affine3f& getCameraFrame() override;
protected:
	void updateCameraFrame();										//更新相机姿态
	Eigen::Vector3f getCameraPos() const;
private:
	Eigen::Affine3f m_transform = Eigen::Affine3f::Identity();
};
