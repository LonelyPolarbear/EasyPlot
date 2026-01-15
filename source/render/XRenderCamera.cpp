#include "XRenderCamera.h"
#include <lib02_camera/xcamera.h>
#include "XRender.h"

XRenderCamera::XRenderCamera()
{
}

XRenderCamera::~XRenderCamera()
{
}

void XRenderCamera::Init()
{
	mCamera = makeShareDbObject<xcamera>();
	//XQ_ATTR_ADD_INIT(AttrProjectionType, (uint32_t)xcamera::cameraType::perspective);
	//XQ_ATTR_ADD_INIT(AttrFovy, 45);
	//XQ_ATTR_ADD_INIT(AttrZnear, 0.1);
	//XQ_ATTR_ADD_INIT(AttrZfar, 5000);
	//XQ_ATTR_ADD_INIT(AttrAspect, 4./3.);
}

void XRenderCamera::setProjectionType(XRenderCamera::ProjectionType type)
{
	if (type == XRenderCamera::ProjectionType::ortho) {
		mCamera->setType(xcamera::cameraType::ortho);
	}
	else {
		mCamera->setType(xcamera::cameraType::perspective);
	}
}

void XRenderCamera::rotate(XQ::Vec2i curPoint, XQ::Vec2i lastPoint, float width, float height)
{
	mCamera->transformTrackball(
		Eigen::Vector2f(curPoint[0], curPoint[1]),
		Eigen::Vector2f(lastPoint[0], lastPoint[1]),
		width,
		height,
		true, true
	);
}

void XRenderCamera::translate(XQ::Vec2i curPoint, XQ::Vec2i lastPoint, float width, float height)
{
	mCamera->transformTrackball(
		Eigen::Vector2f(curPoint[0], curPoint[1]),
		Eigen::Vector2f(lastPoint[0], lastPoint[1]),
		width,
		height,
		false, true
	);
}

void XRenderCamera::resetCamera(XQ::BoundBox box)
{
	mCamera->resetCamera((double*)&box);
}

void XRenderCamera::scale(float factor)
{
	mCamera->scale(factor);
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

void XRenderCamera::render(sptr<XRender> render)
{
}