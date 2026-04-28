#pragma once
#include "../xrendernodeApi.h"
#include "xshapeSource.h"
#include <filesystem>

class xrendernode_API XCustomSource : public XShapeSource {
protected:
	XCustomSource();

	~XCustomSource();

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