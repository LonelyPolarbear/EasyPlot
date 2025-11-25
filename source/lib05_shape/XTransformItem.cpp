#include "XTransformItem.h"
#include <lib04_opengl/XOpenGLBuffer.h>
#include <lib01_shader/xshaderManger.h>

XTransformItem::XTransformItem(std::shared_ptr<XGraphicsItem> parent):XGraphicsItem(parent)
{
	
}

XTransformItem::~XTransformItem()
{
}

void XTransformItem::draw(const Eigen::Matrix4f& m)
{
	Eigen::Matrix4f mat = m * getTransform().matrix();
	for (auto item : m_childItems) {
		item->draw(mat);
	}
}

void XTransformItem::drawFill(std::shared_ptr<xshader> fill_shader, const Eigen::Matrix4f& m)
{
	
}

void XTransformItem::drawBorderImpl(std::shared_ptr<xshader> border_shader, const Eigen::Matrix4f& m, bool)
{
}

void XTransformItem::pickFillDraw(std::shared_ptr<xshader> shader, const Eigen::Matrix4f& m)
{
	return XGraphicsItem::pickFillDraw(shader, m);
}

void XTransformItem::pickBorderDraw(std::shared_ptr<xshader> shader, const Eigen::Matrix4f& m)
{
	return XGraphicsItem::pickBorderDraw(shader, m);
}
