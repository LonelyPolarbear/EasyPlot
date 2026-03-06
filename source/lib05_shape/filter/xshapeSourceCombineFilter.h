#pragma once
#include "../lib05_shapeApi.h"
#include "../datasource/xshapeSource.h"
#include <lib00_utilty/XUtilty.h>


class LIB05_SHAPE_API XShapeSourceCombineFilter : public XShapeSource {
protected:
	XShapeSourceCombineFilter();

	~XShapeSourceCombineFilter();
public:
	void addInput(sptr<XShapeSource> input);

	bool update() override;

	virtual void updateVertextCoordArray() {};

	virtual void updateVertextNormalArray() {};

	virtual void updateVertexColorArray() {};

	virtual void updateFaceIndexArray() {};

	virtual void updateFaceColorArray() {};

	virtual void updateLineIndexArray() {};

	virtual void updateLineColorArray() {};

	virtual void updateVertexIndexArray() {};

protected:
	bool updateSelf();
	class Internal;
	std::shared_ptr<Internal> mData;
};