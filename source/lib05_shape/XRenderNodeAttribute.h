#pragma once
#include "xshapeApi.h"
#include <dataBase/XDataObject.h>
#include <dataBase/XDataAttribute.h>
#include <xsignal/XSignal.h>
#include <Eigen/Eigen>

enum  class LIB05_SHAPE_API graphicsItemType {
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


enum class LIB05_SHAPE_API  ColorMode {
	SingleColor = 1,
	VertexColor = 2,
	FaceColor = 3,
	textureColor = 4,
	SelectTest = 5						//”√”⁄ ∞»°≈–∂œ
};

enum class LIB05_SHAPE_API  PolygonMode {
	point = 1,
	line = 1 << 1,
	face = 1 << 2,
	point_and_line = point | line,
	point_and_face = point | face,
	line_and_face = line | face,
	all = point | line | face
};

class LIB05_SHAPE_API XRenderNodeAttribute :public XDataObject {

};

class LIB05_SHAPE_API XRenderNode3DAttribute :public XDataObject {
protected:
	XRenderNode3DAttribute();
	~XRenderNode3DAttribute();
public:
	void Init();
public:
	csptr<XAttr_Enum<PolygonMode>>  polygonMode;
};