#include "XInfinitePlaneRenderNode.h"

XInfinitePlaneRenderNode::XInfinitePlaneRenderNode()
{
}

XInfinitePlaneRenderNode::~XInfinitePlaneRenderNode()
{
}

void XInfinitePlaneRenderNode::createSource()
{
	m_inputSource = makeShareDbObject<XCustomSource>();
	auto coord = m_inputSource->getVertextCoordArray();
	coord->setNumOfTuple(4);
	coord->setTuple(0, -1, -1, -1);
	coord->setTuple(1, 1, -1, -1);
	coord->setTuple(2, 1, 1, -1);
	coord->setTuple(3, -1, 1, -1);
	coord->Modified();

	auto index = m_inputSource->getIndexArray();
	index->setNumOfTuple(6);
	index->setTuple(0, 0, 1, 2);
	index->setTuple(1, 0, 2, 3);
	index->Modified();

	m_inputSource->Modified();

	this->setColorMode(ColorMode::SingleColor);
	this->setSingleColor(XQ::Vec4f(1, 1, 1, 1));

	this->setInput(m_inputSource);
}

void XInfinitePlaneRenderNode::Init()
{
	XGeometryNode::Init();

	//!
	//! source´´½¨
	createSource();
}

void XInfinitePlaneRenderNode::setFrame(const Eigen::Matrix4f& frame)
{
	m_planeFrame = frame;
}

void XInfinitePlaneRenderNode::draw()
{
	
}
