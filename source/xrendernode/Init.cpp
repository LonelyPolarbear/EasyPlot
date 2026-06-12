#include "Init.h"
#include <dataBase/XObjectFactory.h>
#include "XRenderNode.h"
#include "XGeometryNode.h"
#include "XRenderNodeAttribute.h"
#include "datasource/xtextSource.h"

static void InitBaseObjectMeta();

static void InitAttrSerialize();

static void InitDataSerialize();

static void InitAttrToString();

XRENDERNODE_API void InitRenderNode()
{
	InitBaseObjectMeta();
	InitAttrToString();

}

static void InitBaseObjectMeta() {

	XBaseObjectMeta::registerObject<XRenderNode>();
	XBaseObjectMeta::registerObject<XDataListT<XRenderNode>>();
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
	XBaseObjectMeta::registerObject<XAttr_Enum<VAlign>>();
	XBaseObjectMeta::registerObject<XAttr_Enum<HAlign>>();
	XBaseObjectMeta::registerObject<XAttr_Enum<ColorMode>>();
	XBaseObjectMeta::registerObject<XAttr_Enum<PrimitveType>>();
	XBaseObjectMeta::registerObject<XAttr_Enum<XRenderNodeOriginPositionType>>();
	XBaseObjectMeta::registerObject<XAttr_Enum<XRenderNodeOriginPositionOrien>>();
}


static void InitAttrToString() {
	XattrToQstringFactory.registerProcessor<1>(XQ_META::ClassName<XAttr_Enum<PolygonMode>>(), XattrEnumToString<PolygonMode>);
	XattrToQstringFactory.registerProcessor<1>(XQ_META::ClassName<XAttr_Enum<ColorMode>>(), XattrEnumToString<ColorMode>);
	XattrToQstringFactory.registerProcessor<1>(XQ_META::ClassName<XAttr_Enum<PrimitveType>>(), XattrEnumToString<PrimitveType>);
	XattrToQstringFactory.registerProcessor<1>(XQ_META::ClassName<XAttr_Enum<VAlign>>(), XattrEnumToString<VAlign>);
	XattrToQstringFactory.registerProcessor<1>(XQ_META::ClassName<XAttr_Enum<HAlign>>(), XattrEnumToString<HAlign>);
	XattrToQstringFactory.registerProcessor<1>(XQ_META::ClassName<XAttr_Enum<XRenderNodeOriginPositionType>>(), XattrEnumToString<XRenderNodeOriginPositionType>);
	XattrToQstringFactory.registerProcessor<1>(XQ_META::ClassName<XAttr_Enum<XRenderNodeOriginPositionOrien>>(), XattrEnumToString<XRenderNodeOriginPositionOrien>);
}
