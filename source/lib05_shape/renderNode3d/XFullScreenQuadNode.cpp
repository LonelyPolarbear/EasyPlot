#include "XFullScreenQuadNode.h"
#include <lib01_shader/xshaderManger.h>

XFullScreenQuadNode::XFullScreenQuadNode()
{
}

XFullScreenQuadNode::~XFullScreenQuadNode()
{
}

void XFullScreenQuadNode::createSource()
{
	m_inputSource = makeShareDbObject<XCustomSource>();
	auto coord = m_inputSource->getVertextCoordArray();
	coord->setNumOfTuple(4);
	coord->setTuple(0, -1, -1, -1);
	coord->setTuple(1, 1, -1, -1);
	coord->setTuple(2, 1, 1, -1);
	coord->setTuple(3, -1, 1, -1);
	coord->Modified();

	auto index = m_inputSource->getFaceIndexArray();
	index->setNumOfTuple(6);
	index->setTuple(0, 0, 1, 2);
	index->setTuple(1, 0, 2, 3);
	index->Modified();

	/*
	3------2
	|        |
	0------1
	*/
	auto vertex_color_array = m_inputSource->getVertexColorArray();
	vertex_color_array->setNumOfTuple(4);	
	vertex_color_array->setTuple(0, 0, 0, 0);
	vertex_color_array->setTuple(1, 0, 0, 0);
	vertex_color_array->setTuple(2, 0, 0, 0);
	vertex_color_array->setTuple(3, 0, 0, 0);
	vertex_color_array->Modified();

	m_inputSource->Modified();

	this->setColorMode(ColorMode::VertexColor);
	this->setSingleColor(XQ::Vec4f(1, 1, 1, 1));

	this->setInput(m_inputSource);
}

void XFullScreenQuadNode::Init()
{
	XGeometryNode::Init();

	//!
	//! source´´½¨
	createSource();
}

void XFullScreenQuadNode::setRect(std::vector<XQ::Vec3f> points)
{
	auto coord = m_inputSource->getVertextCoordArray();
	coord->setTuple(0, points[0].x(), points[0].y(), points[0].z());
	coord->setTuple(1, points[1].x(), points[1].y(), points[1].z());
	coord->setTuple(2, points[2].x(), points[2].y(), points[2].z());
	coord->setTuple(3, points[3].x(), points[3].y(), points[3].z());
	coord->Modified();
	m_inputSource->Modified();
}

void XFullScreenQuadNode::setNearRect()
{
	auto coord = m_inputSource->getVertextCoordArray();
	for (int i = 0; i < coord->getNumOfTuple(); i++) {
		coord->data(i)[2] = -0.9999999;
	}
	m_inputSource->Modified();
}

void XFullScreenQuadNode::setFarRect()
{
	auto coord = m_inputSource->getVertextCoordArray();
	for (int i = 0; i < coord->getNumOfTuple(); i++) {
		coord->data(i)[2] = 0.9999999;
	}
	m_inputSource->Modified();
}

void XFullScreenQuadNode::draw(const Eigen::Matrix4f& parentMatrix, bool isNormal)
{
	if (isNormal) {
		auto shader = getShaderManger()->getNdcShader();
		draw(shader, parentMatrix);
	}
	
}

void XFullScreenQuadNode::draw(std::shared_ptr<xshader> s, const Eigen::Matrix4f& parentMatrix)
{
	XGeometryNode::draw(s, parentMatrix);
}

void XFullScreenQuadNode::setVertexColor(std::vector<XQ::XColor> colors)
{
	auto coord = m_inputSource->getVertexColorArray();
	coord->setTuple(0, colors[0].r2(), colors[0].g2(), colors[0].b2());
	coord->setTuple(1, colors[1].r2(), colors[1].g2(), colors[1].b2());
	coord->setTuple(2, colors[2].r2(), colors[2].g2(), colors[2].b2());
	coord->setTuple(3, colors[3].r2(), colors[3].g2(), colors[3].b2());
	
	coord->Modified();
	m_inputSource->Modified();
}
