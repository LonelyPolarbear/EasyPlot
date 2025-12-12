#pragma once

#ifdef LIB02_CAMERA_DLL
#define lib02_CAMERA_API __declspec(dllexport)
#else
#define lib02_CAMERA_API __declspec(dllimport)
#endif // LIB02_SHADER_DLL

#include <Eigen/Eigen>
#include <dataBase/dataobject.h>
#include <lib00_utilty/myUtilty.h>
class lib02_CAMERA_API xcamera:public DataBaseObject
{
public:
	enum class cameraType {
		perspective,
		ortho
	};

	enum class oriention {
		leftBottom,
		leftTop,
		rightBottom,
		rightTop,
		center
	};
public:
	xcamera();
	~xcamera();

	Eigen::Matrix4f projectionMatrix() const;

	void scale(float sacleFactor,bool isLast = true);

	void transform(float picth, float yaw, float roll,  float tx, float ty, float tz);

	void setEyeDir(const XQ::Vec3f &dir);

	/**
	 * @brief 
	 * @param curPointNorm 屏幕坐标系0-1
	 * @param lastPointNorm 屏幕坐标系0-1
	 */
	void transformTrackball(Eigen::Vector2f curPoint,Eigen::Vector2f lastPoint, float width, float height,bool isRotate,bool isYUP = false);

	//获取始终面对相机方向,位于近平面的一个点
	Eigen::Vector3f billboard(float screenw,float screenh, float posx, float posy, float minus_zInCamera,oriention orien = oriention::leftBottom);

	//对于指定的Z平面，相机切平面与屏幕在水平方向的缩放比例 相机：屏幕宽度
	float scaleFactorH(float zValue, float screenw);

	float scaleFactorHZnear(float screenw);

	//对于指定的Z平面，相机切平面与屏幕在竖直方向的缩放比例 相机：屏幕高度
	float scaleFactorV(float zValue, float screenh);

	float scaleFactorVZnear( float screenh);

	Eigen::Matrix4f perspective() const;

	Eigen::Matrix4f ortho() const;

	void resetCamera(const double boundBox[6]);

	std::vector<XQ::Vec3f> getFrustumInWorld() const;
	XQ::Vec3f getNearPointInWorld() const;
	XQ::Vec3f getFarPointInWorld() const;
private:
	void transformImpl(float picth, float yaw, float roll, float tx, float ty, float tz);
public:
	void setTranslate(float x, float y, float z);
	void setRotateCenter(float x, float y, float z);
	Eigen::Vector3f getRotateCenter() const;
	Eigen::Matrix4f getViewMatrix() const;

	/// <summary>
	/// 屏幕坐标到NDC坐标，屏幕坐标系为左手坐标系，原点位于屏幕左下角，X轴向右，Y轴向上,Z轴朝向屏幕里，范围[-1,1]
	/// </summary>
	Eigen::Vector3f ComputeDisplayToNdc(Eigen::Vector3f input) const;

	/// <summary>
	/// NDC坐标到屏幕坐标，屏幕坐标系为左手坐标系，原点位于屏幕左下角，X轴向右，Y轴向上,Z轴朝向屏幕里，范围[-1,1]
	/// </summary>
	Eigen::Vector3f ComputeNdcToDisplay(Eigen::Vector3f input) const;

	/// <summary>
	/// 世界坐标转屏幕坐标，屏幕坐标系为左手坐标系，原点位于屏幕左下角，X轴向右，Y轴向上,Z轴朝向屏幕里，范围[0,1]
	/// </summary>
	/// <param name="input">世界坐标系下的一点</param>
	/// <returns> 屏幕坐标系下的一点 范围[0,1]</returns>
	Eigen::Vector3f ComputeWorldToDisplay(Eigen::Vector3f input) const;

	/// <summary>
	/// 屏幕坐标转世界坐标，屏幕坐标系为左手坐标系，原点位于屏幕左下角，X轴向右，Y轴向上,Z轴朝向屏幕里，范围[0,1]
	/// </summary>
	/// <param name="input"></param>
	/// <returns></returns>
	Eigen::Vector3f ComputeDisplayToWorld(Eigen::Vector3f input) const;

	/// <summary>
	/// 屏幕坐标转相机坐标系，屏幕坐标系为左手坐标系，原点位于屏幕左下角，X轴向右，Y轴向上,Z轴朝向屏幕里，范围[0,1]
	/// </summary>
	/// <param name="input"></param>
	/// <returns>相机坐标系下的位置</returns>
	Eigen::Vector3f ComputeDisplayToCamera(Eigen::Vector3f input) const;

	Eigen::Vector3f ComputeWorldToCamera(Eigen::Vector3f input) const;

	Eigen::Vector3f ComputeCameraToWorld(Eigen::Vector3f input) const;

	Eigen::Vector3f getPosition() const;

	void setAspect(float aspect) {
		m_aspect = aspect;
	}

	float getAspect() const {
		return m_aspect;
	}

	float getZnear() const {
		return m_znear;
	}

	float getZfar() const {
		return m_zfar;
	}

	cameraType getType() const {
		return m_type;
	}

	void setType(cameraType type) {
		m_type = type;
	}

	float getHeight() const {
		return m_height;
	}

	void setHeight(float height) {
		m_height = height;
	}
public:
	//透视投影
	float m_fovy = 45;
	float m_znear = 0.1;
	float m_zfar = 5000;
	float m_aspect = 4. / 3.;

	//正交投影时候使用 m_znear m_zfar  m_aspect m_height
	float m_height = 500;		

	cameraType m_type = cameraType::ortho;
	
private:
	Eigen::Vector3f rotate_center{ 0,0,0 };
	Eigen::Affine3f m_transform = Eigen::Affine3f::Identity();
public:
};
