#include "XAxisItem.h"
#include "XLineItem.h"
#include "XTextItem.h"

XAxisItem::XAxisItem(std::shared_ptr<XGraphicsItem> parent) :XCompositeItem(parent)
{
	//setIsComposite(true);
	m_line = makeShareDbObject<XLineItem>(nullptr);

	if (mLayout == XGL::Layout::horizontal) {
		m_line->setLine(XQ::Vec2f(0, 0), XQ::Vec2f(1, 0));
	}
	else {
		m_line->setLine(XQ::Vec2f(0, 0), XQ::Vec2f(0, 1));
	}
	addChildItem(m_line);

	setLabelNum(5);
}

XAxisItem::~XAxisItem()
{
}

void XAxisItem::updateData()
{
	m_line->updateData();
}



void XAxisItem::setRange(double minVal, double maxVal)
{
	mRange.x() = minVal;
	mRange.y() = maxVal;

	auto length = maxVal - minVal;
	for (int i = 0; i < mLabelNum; i++) {
		auto text = m_texts[i];
		auto step = length / (mLabelNum - 1);
		double value = minVal + i * step;
		text->setText(XQ::to_wstring_with_precision(value, 1));
	}
}

void XAxisItem::updateTextPos() {
	double len = m_line->getLength();
	auto startPos =m_line->getStart();
	for (int i = 0; i < mLabelNum; i++) {
		auto text = m_texts[i];
		if (mLayout == XGL::Layout::horizontal) {
			text->setVAlignment(XTextItem::VAlign::Top);
			if (i + 1 == mLabelNum)
				text->setHAlignment(XTextItem::HAlign::Right);
			else
				text->setHAlignment(XTextItem::HAlign::Left);

			double xpos = startPos.x() + (double)i * len / (mLabelNum - 1);
			text->setPosition(xpos, startPos.y());
		}
		else {
			text->setHAlignment(XTextItem::HAlign::Left);
			if (i + 1 == mLabelNum)
				text->setVAlignment(XTextItem::VAlign::Top);
			else
				text->setVAlignment(XTextItem::VAlign::Bottom);
			double ypos = startPos.y() + (double)i * len / (mLabelNum - 1);
			text->setPosition(startPos.x(), ypos);
		}
	}
}

void XAxisItem::setLabelNum(int num)
{
	//先清空旧的
	for (auto text : m_texts)
		m_line->removeChildItem(text);
	m_texts.clear();

	mLabelNum = num;

	for (int i = 0; i < mLabelNum; i++) {
		auto text = makeShareDbObject<XTextItem>(m_line);
		text->setVisible(true);
		text->setFontSize(16);
		text->setPositionType(XGL::PositionType::local_center);		//或者sceneScreen_center
		text->setSingleColor(XQ::Vec4f(1, 0, 0, 1));
		text->setText(L"0");

		m_texts.push_back(text);
		m_line->addChildItem(text);
	}

	updateTextPos();
}

void XAxisItem::setLayout(XGL::Layout layout)
{
	mLayout = layout;
	if (mLayout == XGL::Layout::horizontal) {
		m_line->setLine(XQ::Vec2f(0, 0), XQ::Vec2f(1, 0));
	}
	else {
		m_line->setLine(XQ::Vec2f(0, 0), XQ::Vec2f(0, 1));
	}
}
