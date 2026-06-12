#include "XRenderCamera.h"
#include <lib02_camera/XTrackBallCamera.h>
#include <xalgo/XAlgo.h>
#include "XRender.h"

XRenderCamera::XRenderCamera()
{
}

XRenderCamera::~XRenderCamera()
{
}

void XRenderCamera::Init()
{
	XBaseRenderCamera::Init();
	mCamera = makeShareDbObject<XTrackBallCamera>();
	sigDataChanged(asDerived<XDataObject>(), XDataChangeType::DataModified);
}

void XRenderCamera::setProjectionType(XBaseRenderCamera::ProjectionType type)
{
	if (type == XBaseRenderCamera::ProjectionType::ortho) {
		mCamera->setType(XBaseCmaera::cameraType::ortho);
	}
	else {
		mCamera->setType(XBaseCmaera::cameraType::perspective);
	}
}

void XRenderCamera::setEyeDir(const XQ::Vec3f& dir)
{
	mCamera->setEyeDir(dir);
}

void XRenderCamera::rotate(XQ::Vec2i curPoint, XQ::Vec2i lastPoint, float width, float height)
{
	if (AttrCameraStyle->getValue() == XBaseRenderCamera::CameraStyle::freely) {
		mCamera->transformFreely(
			Eigen::Vector2f(curPoint[0], curPoint[1]),
			Eigen::Vector2f(lastPoint[0], lastPoint[1]),
			width,
			height,
			true, true
		);
	}
	else {
		mCamera->transformUpright(
			Eigen::Vector2f(curPoint[0], curPoint[1]),
			Eigen::Vector2f(lastPoint[0], lastPoint[1]),
			width,
			height,
			true, true
		);
	}
	sigDataChanged(asDerived<XDataObject>(), XDataChangeType::DataModified);
}

void XRenderCamera::translate(XQ::Vec2i curPoint, XQ::Vec2i lastPoint, float width, float height)
{
	if (AttrCameraStyle->getValue() == XBaseRenderCamera::CameraStyle::freely)
	{
		mCamera->transformFreely(
			Eigen::Vector2f(curPoint[0], curPoint[1]),
			Eigen::Vector2f(lastPoint[0], lastPoint[1]),
			width,
			height,
			false, true
		);
	}
	else {
		mCamera->transformUpright(
			Eigen::Vector2f(curPoint[0], curPoint[1]),
			Eigen::Vector2f(lastPoint[0], lastPoint[1]),
			width,
			height,
			false, true
		);
	}
	sigDataChanged(asDerived<XDataObject>(), XDataChangeType::DataModified);
}

void XRenderCamera::resetCamera(XQ::BoundBox box)
{
	mCamera->resetCamera((double*)&box);
	sigDataChanged(asDerived<XDataObject>(), XDataChangeType::DataModified);
}

void XRenderCamera::scale(float factor)
{
	mCamera->scale(factor);
	sigDataChanged(asDerived<XDataObject>(),XDataChangeType::DataModified);
}

XRenderCamera::ProjectionType XRenderCamera::getProjectionType() const
{
	if(mCamera->getType() == XBaseCmaera::cameraType::ortho)
		return ProjectionType::ortho;
	else
		return ProjectionType::perspective;
}

void XRenderCamera::setNear(double n)
{
	mCamera->setNear(n);
	sigDataChanged(asDerived<XDataObject>(), XDataChangeType::DataModified);
}

void XRenderCamera::setFar(double f)
{
	mCamera->setFar(f);
	sigDataChanged(asDerived<XDataObject>(), XDataChangeType::DataModified);
}

double XRenderCamera::getNear() const
{
	return mCamera->getZnear();
}

double XRenderCamera::getFar() const
{
	return mCamera->getZfar();
}

Eigen::Matrix4f XRenderCamera::projectionMatrix() const
{
	return mCamera->projectionMatrix();
}

Eigen::Matrix4f XRenderCamera::getViewMatrix() const
{
	return mCamera->getViewMatrix();
}

Eigen::Affine3f& XRenderCamera::getCameraFrame()
{
	return mCamera->getCameraFrame();
}

Eigen::Matrix4f XRenderCamera::orthoMatrix(float width, float height, float znear, float zfar) const
{
	return XQ::Matrix::ortho(-0.5 * width, 0.5 * width, -0.5 * height, 0.5 * height, znear, zfar);
}

Eigen::Matrix4f XRenderCamera::perspectiveMatrix(float aspect, float fovy, float znear, float zfar) const
{
	return XQ::Matrix::perspective(fovy, aspect, znear, zfar);
}

Eigen::Matrix4f XRenderCamera::getNearFrame(sptr<XBaseRender> render) const
{
	//在相机的近平面中心创建一个坐标系，X轴向左，Y轴向上，近平面宽对应视口宽 近平面高对应视口高度
	auto nearSize = getNearSizeInCamera();
	auto viewport = render->getConvertViewPort();
	auto viewport_w = viewport[2];
	auto viewport_h = viewport[3];

	auto scale_x = (float)viewport_w / nearSize[0];
	auto scale_y = (float)viewport_h / nearSize[1];
	Eigen::Affine3f frame = Eigen::Affine3f::Identity();
	frame.translate(Eigen::Vector3f(0,0,-getNear()));
	frame.scale(Eigen::Vector3f(1./scale_x,1./scale_y,1));
	return frame.matrix();
}

XQ::Vec2f XRenderCamera::getNearSizeInCamera() const
{
	return mCamera->getNearSizeInCamera();
}

XQ::Vec2f XRenderCamera::getFarSizeInCamera() const
{
	return mCamera->getFarSizeInCamera();
}

void XRenderCamera::setAspect(float aspect)
{
	mCamera->setAspect(aspect);
	sigDataChanged(asDerived<XDataObject>(), XDataChangeType::DataModified);
}

double XRenderCamera::getAspect() const
{
	return mCamera->getAspect();
}

std::vector<XQ::Vec3f> XRenderCamera::getFrustumInWorld() const
{
	return mCamera->getFrustumInWorld();
}

float XRenderCamera::scaleFactorH(float zValue, float screenw)
{
	return mCamera->scaleFactorH(zValue,screenw);
}

float XRenderCamera::scaleFactorV(float zValue, float screenh)
{
	return mCamera->scaleFactorV(zValue, screenh);
}

Eigen::Vector3f XRenderCamera::ComputeDisplayToNdc(Eigen::Vector3f input) const
{
	return mCamera->ComputeDisplayToNdc(input);
}

Eigen::Vector3f XRenderCamera::ComputeNdcToDisplay(Eigen::Vector3f input) const
{
	return mCamera->ComputeNdcToDisplay(input);
}

Eigen::Vector3f XRenderCamera::ComputeWorldToDisplay(Eigen::Vector3f input) const
{
	return mCamera->ComputeWorldToDisplay(input);
}

Eigen::Vector3f XRenderCamera::ComputeDisplayToWorld(Eigen::Vector3f input) const
{
	return mCamera->ComputeDisplayToWorld(input);
}

Eigen::Vector3f XRenderCamera::ComputeDisplayToCamera(Eigen::Vector3f input) const
{
	return mCamera->ComputeDisplayToCamera(input);
}

Eigen::Vector3f XRenderCamera::ComputeWorldToCamera(Eigen::Vector3f input) const
{
	return mCamera->ComputeWorldToCamera(input);
}

Eigen::Vector3f XRenderCamera::ComputeCameraToWorld(Eigen::Vector3f input) const
{
	return mCamera->ComputeCameraToWorld(input);
}

std::array< XQ::Vec3f, 4> XRenderCamera::getFrustumIntersections(const Eigen::Matrix4f& m, int type/*0 1 2 表示 X Y Z平面*/)
{

	auto points =getFrustumInWorld();		//前4个是近平面的点 后四个是远平面的点
	//将这些点转换到待绘制网格的坐标系下
	Eigen::Affine3f affine;
	affine.matrix() = m.inverse();
	
	for (auto& s : points) {
		auto t1 =affine*Eigen::Vector3f(s[0],s[1],s[2]);
		s[0] = t1[0];
		s[1] = t1[1];
		s[2] = t1[2];
	}
	XQ::Vec3f nearPoint[4];
	XQ::Vec3f fatPoint[4];

	nearPoint[0] = points[0];
	nearPoint[1] = points[1];
	nearPoint[2] = points[2];
	nearPoint[3] = points[3];

	fatPoint[0] = points[4];
	fatPoint[1] = points[5];
	fatPoint[2] = points[6];
	fatPoint[3] = points[7];

	std::array< XQ::Vec3f, 4> result;
	if (type == 0) {
		result = XQ::XAlgo::getFrustumYOZIntersections(nearPoint,fatPoint);
	}
	if (type == 1) {
		result = XQ::XAlgo::getFrustumXOZIntersections(nearPoint, fatPoint);
	}
	if (type == 2) {
		result = XQ::XAlgo::getFrustumXOYIntersections(nearPoint, fatPoint);
	}
	
	return result;
}

void XRenderCamera::render(sptr<XRender> render)
{
}