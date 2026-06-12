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

	void adjustScreenCameraPos(sptr<XBaseRender>  render, std::shared_ptr<xshader>, const Eigen::Matrix4f& parentMatrix) override;
public:
	void draw(sptr<XBaseRender>  render, const Eigen::Matrix4f& parentMatrix) override;
	void Init() override;

	void setTextColor(const XQ::XColor& c);
	void setText(const std::string& text);
	void setFontSize(double fontSize);
	void setFixedWidth(double width);
	void setIsFixed(bool isFixed); 
	void setTextHorAlignment(HAlign alignment);
	void setTextVerAlignment(VAlign alignment);

	std::wstring getText() const;
	double getFontSize() const;
	double getFixedWidth() const;
	bool isFixedWidth() const;
	HAlign getTextHorAlignment() const;
	VAlign getTextVerAlignment() const;
public:
	/*¹«¹²ÊôÐÔ*/
	csptr<XAttr_String> AttrText;
	csptr<XAttr_Int> AttrFontSize;
	csptr<XAttr_Bool> AttrIsFixedWidth;
	csptr<XAttr_Double> AttrFixedWidth;
	csptr<XAttr_Enum<VAlign>> AttrVerAlign;
	csptr<XAttr_Enum<HAlign>> AttrHorAlign;
	csptr<XAttr_Float> AttrMaxDerivate;
	csptr<XAttr_Float> AttrMinDerivate;
protected:
	class Internal;
	std::unique_ptr<Internal> mData;
};