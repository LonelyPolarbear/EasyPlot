#pragma once
#include <xrendernode/XGeometryNode.h>
#include "xrendernode/datasource/xtextSource.h"

class XPolyDataMapper;
class XRENDERNODE_API XTextRenderNode :public XGeometryNode {
	REGISTER_CLASS_META_DATA(XTextRenderNode, XGeometryNode);
protected:
	XTextRenderNode();
	virtual ~XTextRenderNode();

	sptr<XPolyDataMapper> getOrCreateMapper() override;
	
	void createSource();
public:
	void draw(sptr<XBaseRender>  render, const Eigen::Matrix4f& parentMatrix) override;
	void Init() override;

	void setText(const std::wstring& text);
	void setFontSize(double fontSize);
	void setFixedWidth(double width);
	void setIsFixed(bool isFixed);

	std::wstring getText() const;
	double getFontSize() const;
	double getFixedWidth() const;
	bool isFixedWidth() const;

	csptr<XAttr_String> AttrText;
protected:
	class Internal;
	std::unique_ptr<Internal> mData;
};