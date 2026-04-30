#include "XBaseRenderCamera.h"

XBaseRenderCamera::XBaseRenderCamera()
{
}

XBaseRenderCamera::~XBaseRenderCamera()
{
}

void XBaseRenderCamera::Init()
{
	XDataObject::Init();
	XQ_ATTR_ADD_INIT(AttrCameraStyle, XBaseRenderCamera::CameraStyle::upRight);
}
