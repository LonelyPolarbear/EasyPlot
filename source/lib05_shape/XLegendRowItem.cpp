#include "XLegendRowItem.h"
#include <lib04_opengl/XOpenGLBuffer.h>
#include "XTextItem.h"
#include "XRectItem.h"
#include "XCheckBoxItem.h"
class XLegendRowItem::Internal {
public:
	Internal() {
		textItem = makeShareDbObject<XTextItem>();
		checkboxItem = makeShareDbObject<XCheckBoxItem>();
		backGrounditem = makeShareDbObject<XRectItem>();

		//复选框设置
		checkboxItem->setChecked(true);

		//背景颜色设置
		//backGrounditem->setSingleColor(myUtilty::Vec4f(0, 1, 1, 1));
		backGrounditem->setIsFilled(true);
		backGrounditem->setFillColor(myUtilty::Vec4f(0, 1, 1, 1));

		//文字设置
		textItem->setSingleColor(myUtilty::Vec4f(1, 0, 0, 1));
		textItem->setFontSize(textSize);
		textItem->setText(plotText);
		textItem->setVAlignment(XTextItem::VAlign::Middle);
		textItem->setHAlignment(XTextItem::HAlign::Left);
		textItem->setPositionType(XGL::PositionType::local_center);
		textItem->setFixedWidth(2);
		textItem->setPosition(0, 0);
	}
		
	std::shared_ptr<XTextItem> textItem;
	std::shared_ptr<XCheckBoxItem> checkboxItem;
	std::shared_ptr<XRectItem> backGrounditem;
	bool checked = false;
	std::wstring plotText =L"测试曲线";

	//宽度控制
	//复选框大小
	double checkboxWidth = 20;
	double checkboxHeight = 20;

	double bacgroundWidth = 200;
	double bacgroundHeight = 20;

	double textSize = 20;

};
XLegendRowItem::XLegendRowItem(std::shared_ptr<XGraphicsItem> parent):XRectItem(parent), m_internal(new Internal)
{
	this->addChildItem(m_internal->checkboxItem);
	this->addChildItem(m_internal->backGrounditem);
	this->addChildItem(m_internal->textItem);
}

XLegendRowItem::~XLegendRowItem()
{
	
}

void XLegendRowItem::setChecked(bool checked)
{
	/*m_internal->checked = checked;
	if (checked) {
		m_internal->textItem->setText(L"√");
	}
	else {
		m_internal->textItem->setText(L" ");
	}*/
}

bool XLegendRowItem::isChecked() const
{
	return m_internal->checked;
}

void XLegendRowItem::updateChildPosition(const Eigen::Matrix4f& m)
{
	auto selfTransform = this->getTransform();
	auto scenFrameInVirtual =getSceneMatrix();

	LocalCoordCompute localCoord(scenFrameInVirtual * m * selfTransform.matrix());
	auto sx =localCoord.scalex;
	auto sy =localCoord.scaley;

	//sx * x =20*0.5
	double spacer = 5;

	//先计算复选框
	auto checkbox_sx = m_internal->checkboxWidth * 0.5 / sx;
	auto checkbox_sy = m_internal->checkboxWidth * 0.5 / sy;
	auto checkbox_translate = localCoord.compute(XGL::HAlignment::Left, m_internal->checkboxWidth * 0.5+ spacer, 0);
	m_internal->checkboxItem->setScale(checkbox_sx, checkbox_sy);
	m_internal->checkboxItem->setPosition(checkbox_translate.x(), checkbox_translate.y());


	//计算颜色块
	auto bacground_sx = m_internal->bacgroundWidth*0.5 / sx;
	auto bacground_sy = m_internal->bacgroundHeight*0.5 / sy;
	auto bacground_translate = localCoord.compute(XGL::HAlignment::Left, m_internal->bacgroundWidth * 0.5+m_internal->checkboxWidth+ spacer*2, 0);
	m_internal->backGrounditem->setScale(bacground_sx, bacground_sy);
	m_internal->backGrounditem->setPosition(bacground_translate.x(), bacground_translate.y());

	//计算文字
	auto text_translate = localCoord.compute(XGL::HAlignment::Left, m_internal->bacgroundWidth + m_internal->checkboxWidth+ spacer*3, 0);
	m_internal->textItem->setPosition(text_translate.x(), text_translate.y());
}
