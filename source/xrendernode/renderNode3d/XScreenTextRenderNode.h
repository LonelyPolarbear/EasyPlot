#pragma once
#include <xrendernode/renderNode3d/XTextRenderNode.h>
#include "xrendernode/datasource/xtextSource.h"

class XPolyDataMapper;
class XRENDERNODE_API XScreenTextRenderNode :public XTextRenderNode {
	REGISTER_CLASS_META_DATA(XScreenTextRenderNode, XTextRenderNode);
protected:
	XScreenTextRenderNode();
	virtual ~XScreenTextRenderNode();
	
public:
	void Init() override;
	void setOrien(XRenderNodeOriginPositionOrien orien);
	void setPos(XQ::Vec2i pos);

	class Internal;
	std::unique_ptr<Internal> mData;
};