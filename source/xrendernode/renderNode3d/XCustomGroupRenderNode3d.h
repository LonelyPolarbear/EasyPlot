#pragma once
#include <xrendernode/renderNode3d/XGroupRenderNode3d.h>
#include <xrendernode/datasource/xCustomSource.h>
#include <Eigen/Eigen>

/// <summary>
/// 矩形渲染节点
/// </summary>
class XCustomGroupRenderNode3d : public XGroupRenderNode3d {
	REGISTER_CLASS_META_DATA(XCustomGroupRenderNode3d, XGroupRenderNode);
protected:
	XCustomGroupRenderNode3d();
	virtual ~XCustomGroupRenderNode3d();
public:
	void Init() override;
	void draw(sptr<XBaseRender> render, const Eigen::Matrix4f& parentMatrix) override;

	csptr<XAttr_Bool> AttrGridNodeDrawFirst;		//true表示先画网格，false表示最后绘制网格
};