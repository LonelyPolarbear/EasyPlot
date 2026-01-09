#pragma once
#include "../xshapeApi.h"
#include "xshapeSource.h"
#include <filesystem>

class LIB05_SHAPE_API XCustomSource : public XShapeSource {
protected:
	XCustomSource();

	~XCustomSource();

public:
	void updateVertextCoordArray() override;

	void updateIndexArray()override;

	void updateFaceColorArray()override;

	void updateNormalArray()override;

	void updateVertexColorArray()override;
};