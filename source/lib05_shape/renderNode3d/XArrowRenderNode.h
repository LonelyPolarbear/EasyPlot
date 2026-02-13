#pragma once
#include <lib05_shape/XGeometryNode.h>
#include <Eigen/Eigen>

/// <summary>
/// 箭头渲染节点
/// </summary>
class XArrowRenderNode : public XGeometryNode {
	REGISTER_CLASS_META_DATA(XArrowRenderNode, XGeometryNode);
protected:
	XArrowRenderNode();
	virtual ~XArrowRenderNode();
protected:
	void createSource();
public:
	void Init() override;

	/// <summary>
	/// 设置箭头的大小
	/// </summary>
	/// <param name="r"></param>
	/// <param name="h"></param>
	void setArrowSize(double r, double h);

	/// <summary>
	/// 设置圆柱的大小
	/// </summary>
	/// <param name="r"></param>
	/// <param name="h"></param>
	void setLineSize(double r, double h);

	void draw(const Eigen::Matrix4f& parentMatrix, bool isNormal) override;
protected:
	class Internal;
	sptr<Internal> mData;
};