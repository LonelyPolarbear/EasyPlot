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
	this->setInput(m_inputSource);

	auto coord = m_inputSource->getVertextCoordArray();
	coord->setNumOfTuple(4);
	coord->setTuple(0, -1, -1, -1);
	coord->setTuple(1, 1, -1, -1);
	coord->setTuple(2, 1, 1, -1);
	coord->setTuple(3, -1, 1, -1);
	coord->Modified();

	auto index = m_inputSource->getFaceIndexArray();
	index->setNumOfTuple(2);
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

	auto vertex_texture_array = m_inputSource->getTextureCoordArray();
	vertex_texture_array->setNumOfTuple(4);
	vertex_texture_array->setTuple(0, 0, 0);
	vertex_texture_array->setTuple(1, 1,0);
	vertex_texture_array->setTuple(2, 1,1);
	vertex_texture_array->setTuple(3, 0,1);
	vertex_texture_array->Modified();

	m_inputSource->Modified();

	this->setColorMode(ColorMode::VertexColor);
	this->setSingleColor(XQ::Vec4f(1, 1, 1, 1));
}

void XFullScreenQuadNode::Init()
{
	XGeometryNode::Init();
	Attribute->AttrIsNdc->setValue(true);

	//!
	//! source´´½¨
	createSource();
}

void XFullScreenQuadNode::setNearRect()
{
	auto coord = m_inputSource->getVertextCoordArray();
	for (int i = 0; i < coord->getNumOfTuple(); i++) {
		coord->data(i)[2] = -1;
	}
	m_inputSource->Modified();
}

void XFullScreenQuadNode::setFarRect()
{
	auto coord = m_inputSource->getVertextCoordArray();
	for (int i = 0; i < coord->getNumOfTuple(); i++) {
		coord->data(i)[2] = 1;
	}
	m_inputSource->Modified();
}

void XFullScreenQuadNode::draw(sptr<XBaseRender> render, const Eigen::Matrix4f& parentMatrix)
{
	return XGeometryNode::draw(render, parentMatrix);
}

void XFullScreenQuadNode::draw(sptr<XBaseRender> render, std::shared_ptr<xshader> s, const Eigen::Matrix4f& parentMatrix)
{
	XGeometryNode::draw(render,s, parentMatrix);
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
