#pragma once

#include "XCameraApi.h"

#include <Eigen/Eigen>
#include <dataBase/XDataBaseObject.h>
#include <lib00_utilty/XUtilty.h>
#include <dataBase/XVector.h>

class lib02_CAMERA_API XBaseCmaera:public XDataBaseObject
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
	XBaseCmaera();
	~XBaseCmaera();

	virtual void scale(float sacleFactor) =0;

	virtual void setEyeDir(const XQ::Vec3f &dir) =0;

	/**
	 * @brief 
	 * @param curPointNorm 屏幕坐标系0-1
	 * @param lastPointNorm 屏幕坐标系0-1
	 */
	virtual void transformFreely(Eigen::Vector2f curPoint, Eigen::Vector2f lastPoint, float width, float height, bool isRotate, bool isYUP = false) = 0;

	virtual void transformUpright(Eigen::Vector2f curPoint, Eigen::Vector2f lastPoint, float width, float height, bool isRotate, bool isYUP = false) = 0;

	virtual void resetCamera(const double boundBox[6]) = 0;

	virtual Eigen::Matrix4f getViewMatrix() const;

	virtual Eigen::Affine3f getCameraFrame() const = 0;

	virtual Eigen::Affine3f& getCameraFrame() =0;

	//获取始终面对相机方向,位于近平面的一个点
	Eigen::Vector3f billboard(float screenw,float screenh, float posx, float posy, float minus_zInCamera,oriention orien = oriention::leftBottom);

	//对于指定的Z平面，相机切平面与屏幕在水平方向的缩放比例 相机：屏幕宽度
	float scaleFactorH(float zValue, float screenw);

	float scaleFactorHZnear(float screenw);

	//对于指定的Z平面，相机切平面与屏幕在竖直方向的缩放比例 相机：屏幕高度
	float scaleFactorV(float zValue, float screenh);

	float scaleFactorVZnear( float screenh);

	Eigen::Matrix4f perspective() const;		//透视投影

	Eigen::Matrix4f ortho() const;					//正交投影

	Eigen::Matrix4f projectionMatrix() const;

	std::vector<XQ::Vec3f> getFrustumInWorld() const;

	XQ::Vec3f getNearPointInWorld() const;

	XQ::Vec3f getFarPointInWorld() const;
public:
	void setRotateCenter(float x, float y, float z);

	Eigen::Vector3f getRotateCenter() const;

	Eigen::Vector3f getPosition() const;

	bool checkFloatWithEpsilon(double a, double b, double epsilon = 1e-9) {
		// 排除等于1的情况（考虑精度）
		auto notEqualToOne = [epsilon](double x) {
			return std::abs(x - 1.0) > epsilon;
			};

		return notEqualToOne(a) && notEqualToOne(b) &&
			((a > 1.0 && b < 1.0) || (a < 1.0 && b > 1.0));
	}

	void setAspect(float aspect) {
		m_aspect = aspect;

		//更新宽高比

		if (checkFloatWithEpsilon(aspect, m_HeightWithAspect)) {
			//此时需要更改高度值
			if (aspect > 1) {
				//此时窗口属于宽窗口，原来是窄窗口
				auto minWidth = getHeight() * m_HeightWithAspect;
				setHeight(minWidth);
			}
			else {
				//当前窗口属于窄窗口，原来是宽窗口
				auto minHeight = getHeight();
				setHeight(minHeight / aspect);
			}
		}
		else {
#if 0
			//同类变化，需要处理更窄或者更低的情况
			if (aspect > 1) {
				//说明都是宽屏，此时需要处理高度
				if (m_HeightWithAspect < aspect) {
					//说明宽屏在垂直方向压缩，需要处理，为了防止看不到，应该放大区域


				}
				else {
					//说明宽屏在水平方向压缩，不需要处理
				}
			}
			else {
				//说明都是窄屏，此时需要处理高度
				if (m_HeightWithAspect > aspect) {
					//说明窄屏在水平方向压缩，需要处理
				}
				else {
					//说明窄屏在垂直方向压缩，不需要处理
				}
			}
#endif
		}

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
		m_HeightWithAspect = m_aspect;
	}

public:

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

protected:
	//透视投影
	float m_fovy = 45;
	float m_znear = 0.1;
	float m_zfar = 5000;
	float m_aspect = 4. / 3.;

	//正交投影时候使用 m_znear m_zfar  m_aspect m_height
	float m_height = 500;		
	float m_HeightWithAspect = 4. / 3.;											//应该记录一下更改m_height时候宽高比

	cameraType m_type = cameraType::ortho;
	
private:
	Eigen::Vector3f rotate_center{ 0,0,0 };
};
