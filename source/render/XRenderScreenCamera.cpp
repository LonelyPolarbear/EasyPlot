#include "XRenderScreenCamera.h"
#include "base/xbaserender/baseRender/XBaseRenderCamera.h"

XRenderScreenCamera::XRenderScreenCamera()
{

}

XRenderScreenCamera::~XRenderScreenCamera()
{

}

void XRenderScreenCamera::Init()
{
	XBaseRenderScreenCamera::Init();
}

void XRenderScreenCamera::setCamera(sptr<XBaseRenderCamera> camera)
{
	mNormalCamera = camera;
}

sptr<XBaseRenderCamera> XRenderScreenCamera::getCamera() const
{
	return mNormalCamera.lock();
}

void XRenderScreenCamera::translate(XQ::Vec2i curPoint, XQ::Vec2i lastPoint, float width, float height)
{
	Eigen::Vector3f curpos = mTransform.inverse() * Eigen::Vector3f(curPoint[0], curPoint[1], 0);
	Eigen::Vector3f lastpos = mTransform.inverse() * Eigen::Vector3f(lastPoint[0], lastPoint[1], 0);

	mTransform.translate(curpos-lastpos);
}

void XRenderScreenCamera::resetCamera(XQ::BoundBox box)
{
	
}

void XRenderScreenCamera::scale(float factor)
{

}

void XRenderScreenCamera::scale(float factor, XQ::Vec2i scaleCenter) 
{
	//Eigen::Vector3f center = mTransform.inverse() * Eigen::Vector3f(scaleCenter[0], scaleCenter[1], 0);
	//mTransform.translate(center);
	mTransform.scale(factor);
	//mTransform.translate(-1*center);
}

Eigen::Matrix4f XRenderScreenCamera::getMatrix(sptr<XBaseRender> render) const
{
	return {};
}

Eigen::Matrix4f XRenderScreenCamera::getNearPlaneFrame(sptr<XBaseRender> render) const
{
	auto camera =getCamera();
	if (camera) {
		return camera->getNearFrame(render);
	}
	else {
		Eigen::Matrix4f::Identity();
	}
}

Eigen::Matrix4f XRenderScreenCamera::getVirtualScreenFrame(sptr<XBaseRender> render) const
{
	return mTransform.matrix();
}

