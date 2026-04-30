#pragma once
#include "xrendernodeApi.h"
#include <dataBase/XDataObject.h>
#include <dataBase/XDataAttribute.h>
#include <xsignal/XSignal.h>
#include <Eigen/Eigen>
#include <lib01_shader/xshader.h>

enum  class XRENDERNODE_API graphicsItemType {
	none,
	rect,
	line,
	triangle,
	circle,
	ellipse,
	polyline,
	polygon,
	text
};

enum class XRENDERNODE_API  ColorMode {
	SingleColor = 1,
	VertexColor = 2,
	FaceColor = 3,
	textureColor = 4,
	SelectTest = 5						//”√”⁄ ∞»°≈–∂œ
};

enum class XRENDERNODE_API  PolygonMode {
	point = 1,
	line = 1 << 1,
	face = 1 << 2,
	point_and_line = point | line,
	point_and_face = point | face,
	line_and_face = line | face,
	all = point | line | face
};


extern template class XRENDERNODE_API XDataAttributeEnum<PolygonMode>;
extern template class XRENDERNODE_API XDataAttributeEnum<PrimitveType>;
extern template class XRENDERNODE_API XDataAttributeEnum<ColorMode>;

class XRENDERNODE_API XRenderNodeAttribute :public XDataObject {
	REGISTER_CLASS_META_DATA(XRenderNodeAttribute,XDataObject);
};

class XRENDERNODE_API XRenderNode3DAttribute :public XRenderNodeAttribute {
	REGISTER_CLASS_META_DATA(XRenderNode3DAttribute, XRenderNodeAttribute);
protected:
	XRenderNode3DAttribute();
	~XRenderNode3DAttribute();
public:
	void Init();
public:
	csptr<XAttr_Enum<PolygonMode>>  AttrPolygonMode;
	csptr<XAttr_Enum<ColorMode>>  AttrColorMode;
	csptr<XAttr_Enum<PrimitveType>>  AttrPrimitveType;
	csptr<XAttr_Color>  AttrSingleColor;
	csptr<XAttr_Color>  AttrPreSelectColor;
	//csptr<XAttr_Bool> AttrNodeVisible;
};