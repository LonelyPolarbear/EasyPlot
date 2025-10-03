#pragma once
#include "xshapeApi.h"
#include "xshapeSource.h"

class LIB05_SHAPE_API XCubeSource : public XShapeSource {
protected:
	XCubeSource();

	~XCubeSource();

public:
	void updateVertextCoordArray() override;

	void updateIndexArray()override;

	void updateFaceColorArray()override;

	void updateNormalArray()override;

	void updateVertexColorArray()override;
};