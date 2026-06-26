#pragma once
#include <xrendernode/mapper/XPolyDataMapper.h>

class XPolyDataAjacencyMapper :public XPolyDataMapper {
protected:
	XPolyDataAjacencyMapper();
	~XPolyDataAjacencyMapper();

	

	void bindSSBO() override;

	void InitRenderResource() override;

	void updateData() override;

	void draw(sptr<xshader> shader, PolygonMode mode, PrimitveType type) override;
public:
	void Init() override;
protected:
	class Internal;
	std::unique_ptr<Internal> mData;
};