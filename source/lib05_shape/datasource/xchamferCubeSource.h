#pragma once
#include "../lib05_shapeApi.h"
#include "xshapeSource.h"

class LIB05_SHAPE_API xchamferCubeSource : public XShapeSource {
protected:
	xchamferCubeSource();

	~xchamferCubeSource();

public:
	void updateVertextCoordArray() override;

	void updateVertextNormalArray()override;

	void updateVertexColorArray()override;

	void updateFaceIndexArray()override;

	void updateFaceColorArray()override;

	void updateLineIndexArray() override;

	void updateLineColorArray() override;

	void updateVertexIndexArray() override;
};