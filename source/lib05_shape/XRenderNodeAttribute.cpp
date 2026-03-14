#include "XRenderNodeAttribute.h"
#include "lib05_shapeApi.h"

template class LIB05_SHAPE_API XDataAttributeEnum<PolygonMode>;
template class LIB05_SHAPE_API XDataAttributeEnum<PrimitveType>;
template class LIB05_SHAPE_API XDataAttributeEnum<ColorMode>;

XRenderNode3DAttribute::XRenderNode3DAttribute()
{
}

XRenderNode3DAttribute::~XRenderNode3DAttribute()
{
}

void XRenderNode3DAttribute::Init()
{
	XRenderNodeAttribute::Init();
	XQ_ATTR_ADD_INIT(AttrPolygonMode, PolygonMode::face);
	XQ_ATTR_ADD_INIT(AttrColorMode, ColorMode::SingleColor);
	XQ_ATTR_ADD_INIT(AttrPrimitveType, PrimitveType::triangle);
	XQ_ATTR_ADD_INIT(AttrSingleColor, XQ::XColor(255, 255, 255, 255));
	XQ_ATTR_ADD_INIT(AttrPreSelectColor, XQ::XColor(255, 255, 255, 255));
	XQ_ATTR_ADD_INIT(AttrNodeVisible, true);
}
