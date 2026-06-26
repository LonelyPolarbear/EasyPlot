#pragma once
#include <xrendernode/XGeometryNode.h>

class XPolyLineRenderNode : public XGeometryNode {
	REGISTER_CLASS_META_DATA(XPolyLineRenderNode, XGeometryNode);
protected:
	XPolyLineRenderNode();
	virtual ~XPolyLineRenderNode();
public:
	void Init() override;
	sptr<XFloatArray> getPolygonLineCoords() const;
	sptr<XPolyDataMapper> getOrCreateMapper() override;

	void draw(sptr<XBaseRender>  render, const Eigen::Matrix4f& parentMatrix) override;

	/*ÊôÐÔ*/
	csptr<XAttr_UInt> AttrLineWidth;//Ïß¿í
	csptr<XAttr_Vec3f> AttrPlaneNormal;//Ïß¿í
	csptr<XAttr_Float> AttrOffset;		//Æ«ÒÆ
	csptr<XAttr_Enum<PenStyle>> AttrPenStyle;
protected:
	class Internal;
	std::unique_ptr<Internal> mData;
};