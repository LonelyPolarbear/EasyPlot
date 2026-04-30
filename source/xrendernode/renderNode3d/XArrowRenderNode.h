#pragma once
#include <xrendernode/XGeometryNode.h>
#include <Eigen/Eigen>

/**
 * @brief 箭头渲染节点，默认箭头是Z+方向
 */
class XArrowRenderNode : public XGeometryNode {
	REGISTER_CLASS_META_DATA(XArrowRenderNode, XGeometryNode);
protected:
	XArrowRenderNode();
	virtual ~XArrowRenderNode();
protected:
	void createSource();
public:
	void Init() override;

	/**
	 * @brief 设置箭头大小
	 * @param r 箭头半径
	 * @param h 箭头高度
	 */
	void setArrowSize(double r, double h);

	/**
	 *  @brief 设置圆柱的大小
	 *  @param r 圆柱半径
	 *  @param h 圆柱高度
	 */
	void setLineSize(double r, double h);

	void draw(sptr<XBaseRender> render, const Eigen::Matrix4f& parentMatrix, bool isNormal) override;
protected:
	class Internal;
	sptr<Internal> mData;
};