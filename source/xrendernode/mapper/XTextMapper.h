#pragma once
#include "XPolyDataMapper.h"
class XTextMapper :public XPolyDataMapper {
protected:
	XTextMapper();
	~XTextMapper();
	void InitRenderResource() override;
public:
	void updateData() override;
	
protected:
	class Internal;
	std::unique_ptr<Internal> mData;
};