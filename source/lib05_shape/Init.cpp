#include "Init.h"
#include <dataBase/XObjectFactory.h>
#include "XRenderNode.h"
#include "XGeometryNode.h"
#include "XRenderNodeAttribute.h"

LIB05_SHAPE_API void InitRenderNode()
{
	XBaseObjectMeta::registerObject<XRenderNode>();
	XBaseObjectMeta::registerObject<XDrawableRenderNode>();
	XBaseObjectMeta::registerObject<XGroupRenderNode>();
	XBaseObjectMeta::registerObject<XTransformRenderNode>();
	XBaseObjectMeta::registerObject<XRenderNode3D>();
	XBaseObjectMeta::registerObject<XRenderNode2D>();

	XBaseObjectMeta::registerObject<XGeometryNode>();

	XBaseObjectMeta::registerObject<XRenderNodeAttribute>();


	//
	XBaseObjectMeta::registerObject<XAttr_Enum<PolygonMode>>();
	XBaseObjectMeta::registerObject<XAttr_Enum<ColorMode>>();
	XBaseObjectMeta::registerObject<XAttr_Enum<PrimitveType>>();
}
