#include "XCompositeItem.h"
#include <lib04_opengl/XOpenGLBuffer.h>

XCompositeItem::XCompositeItem(std::shared_ptr<XGraphicsItem> parent):XGraphicsItem(parent)
{
	
}

XCompositeItem::~XCompositeItem()
{
}

void XCompositeItem::draw(const Eigen::Matrix4f& m)
{
	initiallize();
	if (!m_IsVisible || !m_shaderManger)
		return;

	Eigen::Matrix4f mat = m * getTransform().matrix();
	for (auto item : m_childItems) {
		item->draw(mat);
	}
}
