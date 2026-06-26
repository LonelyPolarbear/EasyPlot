#pragma once
#include <xrendernode/XGeometryNode.h>
#include <xrendernode/datasource/xCustomSource.h>
#include <Eigen/Eigen>
class XFinitePlaneRenderNode;
class XPolyLineRenderNode;
class XGroupRenderNode3d;
class XChartRenderNode : public XGeometryNode,public XBaseRenderNodeExtInterface {
	REGISTER_CLASS_META_DATA(XChartRenderNode, XGeometryNode, XBaseRenderNodeExtInterface);
protected:
	XChartRenderNode();
	virtual ~XChartRenderNode();
protected:
	void createSource();
public:
	void Init() override;
	void setRect(std::vector<XQ::Vec3f> points);
	void draw(sptr<XBaseRender> render, const Eigen::Matrix4f& parentMatrix) override;

	void setXRange(float min, float max);
	void setYRange(float min, float max);

	void gridTranslate(float tx, float ty);
	void gridScale(float sx, float sy, const XQ::Vec2f center);
	void addLineNode(sptr<XPolyLineRenderNode> line);
	XQ::BoundBox getBoundBox(const Eigen::Matrix4f& m) const override;
protected:
	void LeftButtonPressEvent(sptr<XBaseRender>, XQ::Vec2i, XQ::KeyboardModifier, XEvent& event, XQ::Vec3f fragcoord) override;
	void LeftButtonReleaseEvent(sptr<XBaseRender>, XQ::Vec2i, XQ::KeyboardModifier, XEvent& event, XQ::Vec3f fragcoord)override;
	void MiddleButtonPressEvent(sptr<XBaseRender>, XQ::Vec2i, XQ::KeyboardModifier, XEvent& event, XQ::Vec3f fragcoord)override {}
	void MiddleButtonReleaseEvent(sptr<XBaseRender>, XQ::Vec2i, XQ::KeyboardModifier, XEvent& event, XQ::Vec3f fragcoord)override {}
	void RightButtonPressEvent(sptr<XBaseRender>, XQ::Vec2i, XQ::KeyboardModifier, XEvent& event, XQ::Vec3f fragcoord)override {}
	void RightButtonReleaseEvent(sptr<XBaseRender>, XQ::Vec2i, XQ::KeyboardModifier, XEvent& event, XQ::Vec3f fragcoord)override {}
	void LeftButtonDoublePressEvent(sptr<XBaseRender>, XQ::Vec2i, XQ::KeyboardModifier, XEvent& event, XQ::Vec3f fragcoord)override {}
	void RightButtonDoublePressEvent(sptr<XBaseRender>, XQ::Vec2i, XQ::KeyboardModifier, XEvent& event, XQ::Vec3f fragcoord) override {}
	void MiddleButtonDoublePressEvent(sptr<XBaseRender>, XQ::Vec2i, XQ::KeyboardModifier, XEvent& event, XQ::Vec3f fragcoord) override {}
	void EnterEvent(sptr<XBaseRender>, XEvent& event, XQ::Vec3f fragcoord)override {}
	void LeaveEvent(sptr<XBaseRender>, XEvent& event, XQ::Vec3f fragcoord)override {}
	void FoucsInEvent(sptr<XBaseRender>, XEvent& event, XQ::Vec3f fragcoord)override {}
	void FoucsOutEvent(sptr<XBaseRender>, XEvent& event, XQ::Vec3f fragcoord)override {}
	void ResizeEvent(sptr<XBaseRender>, XQ::Vec2i, XEvent& event, XQ::Vec3f fragcoord)override {}
	void KeyPressEvent(sptr<XBaseRender>, XQ::Key, XQ::KeyboardModifier, XEvent& event, XQ::Vec3f fragcoord)override {}
	void KeyReleaseEvent(sptr<XBaseRender>, XQ::Key, XQ::KeyboardModifier, XEvent& event, XQ::Vec3f fragcoord) override {}
	void MouseMoveEvent(sptr<XBaseRender>, XQ::Vec2i, XQ::KeyboardModifier, XEvent& event, XQ::Vec3f fragcoord)override;
	void MouseWheelForwardEvent(sptr<XBaseRender>, XQ::Vec2i, XQ::KeyboardModifier, XEvent& event, XQ::Vec3f fragcoord)override ;
	void MouseWheelBackwardEvent(sptr<XBaseRender>, XQ::Vec2i, XQ::KeyboardModifier, XEvent& event, XQ::Vec3f fragcoord)override ;
protected:
	sptr<XCustomSource> m_inputSource;
protected:
	class Internal;
	std::unique_ptr<Internal> mData;
};