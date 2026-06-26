#pragma once
#include "../xrendernodeApi.h"
#include "xshapeSource.h"
#include <filesystem>

/**
 * @class XPolygonLineSource
 * @detail 多端线的source，主要差异是其采用带邻接信息的线段
 */
class XRENDERNODE_API XPolygonLineSource : public XShapeSource {
protected:
	XPolygonLineSource();

	~XPolygonLineSource();

public:
	void updateVertextCoordArray() override;

	void updateVertextNormalArray() override;

	void updateVertexColorArray() override;

	void updateFaceIndexArray()override;

	void updateFaceColorArray()override;

	void updateLineIndexArray() override;

	void updateLineColorArray() override;

	void updateVertexIndexArray() override;

	void updateInstancedArray() override;
	
	void updateTextureCoordArray() override{};	

	void updateCustomArray() override{};

	void setPolygonLineCoords(sptr<XFloatArray> coords);
	sptr<XFloatArray> getPolygonLineCoords() const;
protected:
	class Internal;
	std::unique_ptr<Internal> mData;
};