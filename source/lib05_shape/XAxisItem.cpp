#include "XAxisItem.h"
#include "XLineItem.h"
#include "XTextItem.h"

XAxisItem::XAxisItem() :XGraphicsItem()
{
	setIsComposite(true);
}

XAxisItem::~XAxisItem()
{
}

void XAxisItem::updateData()
{
	m_line->updateData();
}

void XAxisItem::initResource()
{
	m_line = makeShareDbObject<XLineItem>();
	m_line->initResource();
	if (mLayout == XGL::Layout::horizontal) {
		m_line->setLine(myUtilty::Vec2f(0, 0), myUtilty::Vec2f(1, 0));
	}
	else {
		m_line->setLine(myUtilty::Vec2f(0, 0), myUtilty::Vec2f(0, 1));
	}

	addChildItem(m_line);

	for (int i = 0; i < mLabelNum; i++) {
		auto text = makeShareDbObject<XTextItem>();
		text->initResource();
		text->setText(myUtilty::to_wstring_with_precision(i, 2));

		text->setVisible(true);

		if (mLayout == XGL::Layout::horizontal) {
			text->setVAlignment(XTextItem::VAlign::Top);
			if (i + 1 == mLabelNum)
				text->setHAlignment(XTextItem::HAlign::Right);
			else
				text->setHAlignment(XTextItem::HAlign::Left);

			double xpos = (double)i / (mLabelNum - 1);
			text->setPosition(xpos, 0);
		}
		else {
			text->setHAlignment(XTextItem::HAlign::Left);
			if (i + 1 == mLabelNum)
				text->setVAlignment(XTextItem::VAlign::Top);
			else
				text->setVAlignment(XTextItem::VAlign::Bottom);
			double ypos = i / (mLabelNum - 1);
		}

		text->setFontSize(16);
		text->setPositionType(XGL::PositionType::local_center);
		text->setSingleColor(myUtilty::Vec4f(1, 0, 0, 1));
		text->setText(L"0");

		m_texts.push_back(text);
		m_line->addChildItem(text);
	}
}


void XAxisItem::setRange(double minVal, double maxVal)
{
	auto length = maxVal - minVal;
	for (int i = 0; i < mLabelNum; i++) {
		auto text = m_texts[i];
		auto step = length / (mLabelNum - 1);
		double value = minVal + i * step;
		text->setText(myUtilty::to_wstring_with_precision(value, 1));
	}
}