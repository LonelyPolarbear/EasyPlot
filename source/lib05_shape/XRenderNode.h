#pragma once
#include "xshapeApi.h"
#include <dataBase/dataobject.h>


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
	line = 2,
	fill = 3,
	line_fill = 4
};

class LIB05_SHAPE_API XRenderNode :public DataBaseObject {

};

class LIB05_SHAPE_API XRenderNode3D :public XRenderNode {

};