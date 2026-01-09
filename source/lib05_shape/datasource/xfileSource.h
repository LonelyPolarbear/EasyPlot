#pragma once
#include "../xshapeApi.h"
#include "xshapeSource.h"
#include <filesystem>

class LIB05_SHAPE_API XFileSource : public XShapeSource {
protected:
	XFileSource();

	~XFileSource();

public:
	void updateVertextCoordArray() override;

	void updateIndexArray()override;

	void updateFaceColorArray()override;

	void updateNormalArray()override;

	void updateVertexColorArray()override;

	void setFilePath(const std::string& filePath);

	void readFile(const std::string& path);

	void transform();
};