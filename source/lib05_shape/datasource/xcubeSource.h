#pragma once
#include "../lib05_shapeApi.h"
#include "xshapeSource.h"

class LIB05_SHAPE_API XCubeSource : public XShapeSource {
protected:
	XCubeSource();

	~XCubeSource();

public:
	void updateVertextCoordArray() override;

	void updateFaceIndexArray()override;

	void updateFaceColorArray()override;

	void updateVertextNormalArray()override;

	void updateVertexColorArray()override;

	void updateLineIndexArray() override;

	void updateLineColorArray() override;

	void updateVertexIndexArray() override;
};