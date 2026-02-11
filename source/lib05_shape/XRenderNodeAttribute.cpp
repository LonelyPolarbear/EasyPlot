#include "XRenderNodeAttribute.h"
#include "xshapeApi.h"

XRenderNode3DAttribute::XRenderNode3DAttribute()
{
}

XRenderNode3DAttribute::~XRenderNode3DAttribute()
{
}

void XRenderNode3DAttribute::Init()
{
	XQ_ATTR_ADD_INIT(polygonMode,PolygonMode::face);
}
