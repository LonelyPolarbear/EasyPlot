#pragma once
#include "xshapeApi.h"
#include <dataBase/XDataBaseObject.h>
#include <xsignal/XSignal.h>
#include "XRenderNodeAttribute.h"
#include <Eigen/Eigen>

class LIB05_SHAPE_API XRenderNode :public XDataBaseObject {
public:
	virtual void draw(const Eigen::Matrix4f& parentMatrix = Eigen::Matrix4f::Identity())=0;
	virtual void drawInstance()=0;
};

class LIB05_SHAPE_API XRenderNode3D :public XRenderNode {
protected:
	XRenderNode3D();
	~XRenderNode3D();
public:
	int64_t getID() const;
protected:
	uint64_t m_id;
public:
	XSIGNAL(void(sptr<XRenderNode>)) sigBeginRender;
	XSIGNAL(void(sptr<XRenderNode>)) sigEndRender;
};