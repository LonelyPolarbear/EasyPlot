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
	XQ_ATTR_ADD_INIT(AttrPolygonMode, PolygonMode::face);
	XQ_ATTR_ADD_INIT(AttrColorMode, ColorMode::SingleColor);
	XQ_ATTR_ADD_INIT(AttrPrimitveType, PrimitveType::triangle);
	XQ_ATTR_ADD_INIT(AttrSingleColor, XQ::XColor(255, 255, 255, 255));
	XQ_ATTR_ADD_INIT(AttrPreSelectColor, XQ::XColor(255, 255, 255, 255));
	XQ_ATTR_ADD_INIT(AttrVisible, true);
}
