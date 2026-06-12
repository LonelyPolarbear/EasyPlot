#pragma once

#include "renderApi.h"
#include <dataBase/XDataBaseObject.h>
#include <dataBase/XDataObject.h>
#include <dataBase/XDataAttribute.h>
#include <lib00_utilty/XUtilty.h>
#include "base/xbaserender/baseRender/XBaseRenderScreenCamera.h"
class XBaseCmaera;
class XRender;
/**
 * @class XRenderScreenCamera 屏幕相机，始终关联一个3d相机
 */
class  RENDER_API XRenderScreenCamera : public XBaseRenderScreenCamera {
protected:
	XRenderScreenCamera();
    ~XRenderScreenCamera();
    void render(sptr< XRender> render){}
public:
    void Init() override;

	void setCamera(sptr<XBaseRenderCamera> camera) override;

	sptr<XBaseRenderCamera> getCamera() const override;

	void translate(XQ::Vec2i curPoint, XQ::Vec2i lastPoint, float width, float height) override;
		
	void resetCamera(XQ::BoundBox box) override;
		
	void scale(float factor) override;

	virtual void scale(float factor, XQ::Vec2i scaleCenter) override;

	Eigen::Matrix4f getMatrix(sptr<XBaseRender> render) const override;

	Eigen::Matrix4f getNearPlaneFrame(sptr<XBaseRender> render) const override;

	Eigen::Matrix4f getVirtualScreenFrame(sptr<XBaseRender> render) const override;
protected:
	wptr<XBaseRenderCamera> mNormalCamera;
	Eigen::Affine3f mTransform = Eigen::Affine3f::Identity();
 };