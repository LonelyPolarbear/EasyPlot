#include "XTransformItem.h"
#include <lib04_opengl/XOpenGLBuffer.h>

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
