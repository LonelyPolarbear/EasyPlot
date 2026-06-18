#include "XChartRenderNode.h"
#include <lib04_opengl/XOpenGLEnable.h>
#include <lib01_shader/xshaderManger.h>
#include <lib04_opengl/XOpenGLBuffer.h>
#include <xrendernode/renderNode3d/XFinitePlaneRenderNode.h>
#include <base/xbaserender/baseRender/XBaseRenderCamera.h>
#include <xlog/XLogger.h>
class XChartRenderNode::Internal {
public:
	wptr<XFinitePlaneRenderNode> grid;
	bool isPress = false;
	Eigen::Vector3f lastPos = Eigen::Vector3f(0,0,0);

	Eigen::Vector3f mapFragCoord2GridPos(sptr<XBaseRender> render,const XQ::Vec3f& fragcoord) {
		auto node = grid.lock();
		if (!node) {
			return  Eigen::Vector3f(0, 0, 0);
		}
		
		Eigen::Affine3f trans = Eigen::Affine3f::Identity();
		node->getChainTransform(trans);
		auto gridAffine = node->getGraidAffine();
		auto window2grid = trans.inverse() * gridAffine.inverse();
		auto world_point = render->getCamera()->ComputeDisplayToWorld(Eigen::Vector3f(fragcoord[0], fragcoord[1], fragcoord[2]));
		auto gridpos = window2grid * world_point;
		return gridpos;
	}
};

XChartRenderNode::XChartRenderNode():mData(new Internal)
{

}

XChartRenderNode::~XChartRenderNode()
{
}

void XChartRenderNode::createSource()
{
	m_inputSource = makeShareDbObject<XCustomSource>();
	auto coord = m_inputSource->getVertextCoordArray();
	coord->setNumOfTuple(4);
	coord->setTuple(0, -1, -1, 0);
	coord->setTuple(1, 1, -1, 0);
	coord->setTuple(2, 1, 1, 0);
	coord->setTuple(3, -1, 1, 0);
	coord->Modified();

	auto index = m_inputSource->getFaceIndexArray();
	index->setNumOfTuple(6);
	index->setTuple(0, 0, 1, 2);
	index->setTuple(1, 0, 2, 3);
	index->Modified();

	m_inputSource->Modified();

	this->setColorMode(ColorMode::SingleColor);
	this->setSingleColor(XQ::Vec4f(1, 1, 1, 1));

	this->setInput(m_inputSource);
}

void XChartRenderNode::Init()
{
	XGeometryNode::Init();
	{
		auto Grid = makeShareDbObject<XFinitePlaneRenderNode>();
		Grid->setName("coordinate");
		addChildRenderNode(Grid);
		Grid->setXRange(-100, 100);
		Grid->setYRange(-100, 100);
		mData->grid = Grid;
	}
	
	//!
	//! source创建
	createSource();
}

void XChartRenderNode::setRect(std::vector<XQ::Vec3f> points)
{
	auto coord = m_inputSource->getVertextCoordArray();
	coord->setTuple(0, points[0].x(), points[0].y(), points[0].z());
	coord->setTuple(1, points[1].x(), points[1].y(), points[1].z());
	coord->setTuple(2, points[2].x(), points[2].y(), points[2].z());
	coord->setTuple(3, points[3].x(), points[3].y(), points[3].z());
	coord->Modified();
	m_inputSource->Modified();
}

void XChartRenderNode::draw(sptr<XBaseRender> render, const Eigen::Matrix4f& parentMatrix)
{
	XGeometryNode::draw(render,parentMatrix);
}

void XChartRenderNode::draw(sptr<XBaseRender> render, std::shared_ptr<xshader> s, const Eigen::Matrix4f& parentMatrix)
{
	XGeometryNode::draw(render,s, parentMatrix);
}

void XChartRenderNode::setXRange(float min, float max)
{
	if (auto grid = mData->grid.lock()) {
		grid->setXRange(min,max);
	}
}

void XChartRenderNode::setYRange(float min, float max)
{
	if (auto grid = mData->grid.lock()) {
		grid->setYRange(min, max);
	}
}

void XChartRenderNode::gridTranslate(float tx, float ty)
{
	if (auto grid = mData->grid.lock()) {
		grid->gridTranslate(tx,ty);
	}
}

void XChartRenderNode::gridScale(float sx, float sy, const XQ::Vec2f center)
{
	if (auto grid = mData->grid.lock()) {
		grid->gridScale(sx, sy,center);
	}
}

void XChartRenderNode::LeftButtonPressEvent(sptr<XBaseRender> render,XQ::Vec2i, XQ::KeyboardModifier, XEvent& event, XQ::Vec3f fragcoord)
{
	if (auto grid = mData->grid.lock()) {
		//将渲染窗口坐标转换为坐标系的内部坐标
		/*Eigen::Affine3f trans = Eigen::Affine3f::Identity();
		grid->getChainTransform(trans);
		auto gridAffine = grid->getGraidAffine();
		auto window2grid = trans.inverse() * gridAffine.inverse();
		auto world_point = render->getCamera()->ComputeDisplayToWorld(Eigen::Vector3f(fragcoord[0], fragcoord[1], fragcoord[2]));
		auto gridpos = window2grid * world_point;*/
		auto  gridpos = mData->mapFragCoord2GridPos(render,fragcoord);
		mData->lastPos = gridpos;
		//XLOG_INFO("gridpos:{},{}{}", gridpos[0], gridpos[1], gridpos[2]);
		mData->isPress = true;
		event.stopPropagate();
	}
}

void XChartRenderNode::LeftButtonReleaseEvent(sptr<XBaseRender> render, XQ::Vec2i, XQ::KeyboardModifier, XEvent& event, XQ::Vec3f fragcoord)
{
	if (auto grid = mData->grid.lock()) {
		mData->isPress = false;
	}
}

void XChartRenderNode::MouseMoveEvent(sptr<XBaseRender> render, XQ::Vec2i, XQ::KeyboardModifier, XEvent& event, XQ::Vec3f fragcoord)
{
	if (auto grid = mData->grid.lock()) {
		if (mData->isPress) {
			auto curpos = mData->mapFragCoord2GridPos(render, fragcoord);
			auto dis = curpos - mData->lastPos;
			auto tx = dis[0];
			auto ty = dis[1];
			gridTranslate(-tx,-ty);
			//XLOG_INFO("translat:{},{}",-tx,-ty);
			mData->lastPos = curpos;
		}
		//event.stopPropagate();
	}
	
}

void XChartRenderNode::MouseWheelForwardEvent(sptr<XBaseRender> render, XQ::Vec2i windowPos, XQ::KeyboardModifier, XEvent& event, XQ::Vec3f fragcoord)
{
	if (auto grid = mData->grid.lock()) {
		//将渲染窗口坐标转换为坐标系的内部坐标
		
		auto gridpos = mData->mapFragCoord2GridPos(render,fragcoord);
		//前进，网格变稀疏
		gridScale(1/1.1,1/1.1,XQ::Vec2f(gridpos[0],gridpos[1]));

		event.stopPropagate();
	}
}

void XChartRenderNode::MouseWheelBackwardEvent(sptr<XBaseRender> render, XQ::Vec2i windowPos, XQ::KeyboardModifier, XEvent& event, XQ::Vec3f fragcoord)
{
	if (auto grid = mData->grid.lock()) {
		//将渲染窗口坐标转换为坐标系的内部坐标
		auto gridpos = mData->mapFragCoord2GridPos(render, fragcoord);
		gridScale(1.1, 1.1, XQ::Vec2f(gridpos[0], gridpos[1]));
		event.stopPropagate();
	}
}
