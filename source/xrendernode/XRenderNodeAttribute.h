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
	SelectTest = 5						//用于拾取判断
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

enum class XRENDERNODE_API  PickMode {
	none =0,
	point = 1,
	line = 1 << 1,
	face = 1 << 2,
	point_and_line = point | line,		//3
	point_and_face = point | face,		//5
	line_and_face = line | face,			//6
	all = point | line | face					//7
};

enum class XRenderNodeOriginPositionType {
	free,			///< 世界坐标系或者父坐标系
	fixed,		///< 屏幕坐标系下固定位置
};

enum class XRenderNodeOriginPositionOrien {
	left_top,
	left_bottom,
	right_top,
	right_bottom,
	center
};

enum class XRenderNodeCameraMode {
	camera3D =1,
	cameraAxis = 2,
	camera2D =3,
};

class XRENDERNODE_API XRenderNodeSizePolicy :public XDataObject {
	REGISTER_CLASS_META_DATA(XRenderNodeSizePolicy, XDataObject);
protected:
	XRenderNodeSizePolicy();
	~XRenderNodeSizePolicy();
public:
	void Init() override;

	csptr<XAttr_Enum<XRenderNodeOriginPositionType>> AttrPositionType;							///< 是否原点在视口固定位置 
	csptr<XAttr_Bool> AttrIsFixedSize;																							///< 是否大小是固定像素大小
	csptr<XAttr_Bool> AttrIsFixedOrien;																						///< 是否大小是固定朝向，即是否固定朝向屏幕,仅对三维物体生效
	csptr<XAttr_Enum<XRenderNodeOriginPositionOrien>> AttrPositionOrien;							///< 只有在XRenderNodeOriginPositionType ==fixed时候有效
	csptr<XAttr_Vec2i> AttrPositionPos;																							///< 只有在XRenderNodeOriginPositionType ==fixed时候有效
	csptr<XAttr_Vec3i> AttrFixedPixel;																							///< 只有在XRenderNodeOriginPositionType ==fixed时候有效,固定像素大小
};


extern template class XRENDERNODE_API XDataAttributeEnum<PolygonMode>;
extern template class XRENDERNODE_API XDataAttributeEnum<PrimitveType>;
extern template class XRENDERNODE_API XDataAttributeEnum<ColorMode>;
extern template class XRENDERNODE_API XDataAttributeEnum<PickMode>;

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
	csptr<XAttr_Enum<PickMode>>  AttrPickMode;
	csptr<XAttr_Color>  AttrSingleColor;
	csptr<XAttr_Color>  AttrPreSelectColor;
	csptr<XAttr_Color>  AttrSelectedColor;
	csptr<XAttr_Bool>  AttrIsNdc;
	csptr<XAttr_Bool>  AttrDrawOutline;
	csptr<XAttr_Enum<XRenderNodeCameraMode>>  AttrCameraMode;
	csptr<XRenderNodeSizePolicy> AttrSizePolicy;
	csptr<XAttr_Bool> AttrIsValidBoundBox; 
};