#pragma once
#include "../xshapeApi.h"
#include "../datasource/xshapeSource.h"
#include <lib00_utilty/XUtilty.h>


class LIB05_SHAPE_API XShapeSourceCombineFilter : public XShapeSource {
protected:
	XShapeSourceCombineFilter();

	~XShapeSourceCombineFilter();
public:
	
	
	void addInput(sptr<XShapeSource> input);

	bool update() override;

protected:
	bool updateSelf();
	class Internal;
	std::shared_ptr<Internal> mData;
};