#include "XRenderCamera.h"
#include <lib02_camera/XTrackBallCamera.h>
#include "XRender.h"

XRenderCamera::XRenderCamera()
{
}

XRenderCamera::~XRenderCamera()
{
}

void XRenderCamera::Init()
{
	mCamera = makeShareDbObject<XTrackBallCamera>();
	XQ_ATTR_ADD_INIT(AttrCameraStyle, (uint32_t)CameraStyle::upRight);

	sigDataChanged(asDerived<XDataObject>(), XDataChangeType::DataModified);
}

void XRenderCamera::setProjectionType(XRenderCamera::ProjectionType type)
{
	if (type == XRenderCamera::ProjectionType::ortho) {
		mCamera->setType(XBaseCmaera::cameraType::ortho);
	}
	else {
		mCamera->setType(XBaseCmaera::cameraType::perspective);
	}
}

void XRenderCamera::rotate(XQ::Vec2i curPoint, XQ::Vec2i lastPoint, float width, float height)
{
	if (AttrCameraStyle->getValue() == (uint32_t)CameraStyle::freely) {
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
	if (AttrCameraStyle->getValue() == (uint32_t)CameraStyle::freely) 
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

void XRenderCamera::setAspect(float aspect)
{
	mCamera->setAspect(aspect);
}

std::vector<XQ::Vec3f> XRenderCamera::getFrustumInWorld() const
{
	return mCamera->getFrustumInWorld();
}

void XRenderCamera::render(sptr<XRender> render)
{
}