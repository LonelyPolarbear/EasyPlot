#pragma once

#include "base/xbaserender/xbaserenderApi.h"
#include "base/xbaserender/baseRender/XBaseRender.h"
#include <dataBase/XDataObject.h>
#include <Eigen/Eigen>
#include <lib00_utilty/XUtilty.h>

class XBaseRender;
class XBaseRenderCamera;
class XBASERENDER_API XBaseRenderScreenCamera : public XDataObject {
	REGISTER_CLASS_META_DATA(XBaseRenderScreenCamera, XDataObject);
protected:
	XBaseRenderScreenCamera();
	virtual ~XBaseRenderScreenCamera();

public:
	void Init() override;

	virtual void setCamera(sptr<XBaseRenderCamera> camera) =0;

	virtual sptr<XBaseRenderCamera> getCamera() const =0;

	virtual void translate(XQ::Vec2i curPoint, XQ::Vec2i lastPoint, float width, float height) =0;

	virtual void resetCamera(XQ::BoundBox box) =0;

	virtual void scale(float factor) = 0;

	virtual void scale(float factor, XQ::Vec2i scaleCenter) = 0;

	virtual Eigen::Matrix4f getMatrix(sptr<XBaseRender> render) const = 0;

	virtual Eigen::Matrix4f getNearPlaneFrame(sptr<XBaseRender> render) const = 0;

	virtual Eigen::Matrix4f getVirtualScreenFrame(sptr<XBaseRender> render) const = 0;
};