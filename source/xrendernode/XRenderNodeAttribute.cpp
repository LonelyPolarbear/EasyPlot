#include "XRenderNodeAttribute.h"
#include "xrendernodeApi.h"

template class XRENDERNODE_API XDataAttributeEnum<PolygonMode>;
template class XRENDERNODE_API XDataAttributeEnum<PrimitveType>;
template class XRENDERNODE_API XDataAttributeEnum<ColorMode>;
template class XRENDERNODE_API XDataAttributeEnum<PickMode>;

XRenderNodeSizePolicy::XRenderNodeSizePolicy()
{
}

XRenderNodeSizePolicy::~XRenderNodeSizePolicy()
{
}

void XRenderNodeSizePolicy::Init()
{
	XDataObject::Init();
	XQ_ATTR_ADD_INIT(AttrPositionType, XRenderNodeOriginPositionType::free);
	XQ_ATTR_ADD_INIT(AttrIsFixedSize, false);
	XQ_ATTR_ADD_INIT(AttrIsFixedOrien, false);
	XQ_ATTR_ADD_INIT(AttrPositionOrien, XRenderNodeOriginPositionOrien::left_bottom); 
	XQ_ATTR_ADD_INIT(AttrPositionPos, XQ::Vec2i(0, 0));
	XQ_ATTR_ADD_INIT(AttrFixedPixel, XQ::Vec3i(1, 1, 1)); 
}

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
	XQ_ATTR_ADD_INIT(AttrPickMode, PickMode::face);
	XQ_ATTR_ADD_INIT(AttrSingleColor, XQ::XColor(255, 255, 255, 255));
	XQ_ATTR_ADD_INIT(AttrPreSelectColor, XQ::XColor(255, 255, 255, 255));
	XQ_ATTR_ADD_INIT(AttrSelectedColor, XQ::XColor(255, 255, 255, 255));
	XQ_ATTR_ADD_INIT(AttrIsNdc, false);
	XQ_ATTR_ADD_INIT(AttrDrawOutline, false);
	XQ_ATTR_ADD_INIT(AttrUseNormalCamera, true);
	XQ_ATTR_ADD_INIT(AttrIsValidBoundBox, true);
	XQ_XDATA_ADD(AttrSizePolicy);
}
