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
}

void XRenderCamera::render(sptr<XRender> render)
{
}