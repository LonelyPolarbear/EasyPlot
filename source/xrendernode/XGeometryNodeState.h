#pragma once
#include "xrendernodeApi.h"
#include <dataBase/XDataBaseObject.h>
#include "XRenderNode.h"

class XRENDERNODE_API XGeometryNodeState : public XRenderNodeState {
	REGISTER_CLASS_META_DATA(XGeometryNodeState, XRenderNodeState);
public:
	XGeometryNodeState();
	virtual ~XGeometryNodeState();
	void Init() override;
	csptr<XAttr_UInt> AttrFaceNum;
	csptr<XAttr_UInt> AttrLineNum;
	csptr<XAttr_UInt> AttrVertexNum;
	csptr<XAttr_Bool> AttrHasSelect;

	void bindBuffer();
	void setFaceState(int faceIdx,const PrimitiveState& faceState);
	PrimitiveState getFaceState(int faceIdx);
protected:
	class Internal;
	std::unique_ptr<Internal> mData;
};