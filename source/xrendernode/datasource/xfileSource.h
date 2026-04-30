#pragma once
#include "../xrendernodeApi.h"
#include "xshapeSource.h"
#include <filesystem>

class XRENDERNODE_API XFileSource : public XShapeSource {
protected:
	XFileSource();

	~XFileSource();

public:
	void updateVertextCoordArray() override;

	void updateFaceIndexArray()override;

	void updateFaceColorArray()override;

	void updateVertextNormalArray()override;

	void updateVertexColorArray()override;

	void updateLineIndexArray() override;

	void updateLineColorArray() override;

	void updateVertexIndexArray() override;

	void setFilePath(const std::string& filePath);

	void readFile(const std::string& path);

	void transform();
};