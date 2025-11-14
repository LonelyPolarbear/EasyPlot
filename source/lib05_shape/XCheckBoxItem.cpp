#include "XCheckBoxItem.h"
#include <lib04_opengl/XOpenGLBuffer.h>
#include "XTextItem.h"
class XCheckBoxItem::Internal {
public:
	Internal() {
		textItem->setSingleColor(myUtilty::Vec4f(1, 0,0, 1));
		textItem->setFontSize(20);
		textItem->setText(L" ");
		textItem->setVAlignment(XTextItem::VAlign::Middle);
		textItem->setHAlignment(XTextItem::HAlign::Center);
		textItem->setPositionType(XGL::PositionType::local_center);
		textItem->setFixedWidth(2);
		textItem->setPosition(0, 0);
	}
	std::shared_ptr<XTextItem> textItem = makeShareDbObject<XTextItem>();
	bool checked = false;
};
XCheckBoxItem::XCheckBoxItem(std::shared_ptr<XGraphicsItem> parent):XRectItem(parent), m_internal(new Internal)
{
	this->addChildItem(m_internal->textItem);
}

XCheckBoxItem::~XCheckBoxItem()
{
}

void XCheckBoxItem::setChecked(bool checked)
{
	m_internal->checked = checked;
	if (checked) {
		m_internal->textItem->setText(L"¡Ì");
	}
	else {
		m_internal->textItem->setText(L" ");
	}
}

bool XCheckBoxItem::isChecked() const
{
	return m_internal->checked;
}
