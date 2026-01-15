#pragma once
#include "xshapeApi.h"
#include <dataBase/XDataBaseObject.h>


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
	fill = 3
};

class LIB05_SHAPE_API XRenderNode :public XDataBaseObject {

};

class LIB05_SHAPE_API XRenderNode3D :public XRenderNode {
protected:
	XRenderNode3D();
	~XRenderNode3D();
public:
	int64_t getID() const;
protected:
	uint64_t m_id;
};