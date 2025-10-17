#include "ColorPickButton.h"
#include <QPainter>
#include <QColorDialog>
#include <QStyle>
#include<QMouseEvent>

ColorPickerButton::ColorPickerButton(QWidget* parent)
	: QToolButton(parent), m_currentColor(Qt::black)
{
	// 设置按钮图标（使用Qt内置图标）
	m_icon = style()->standardIcon(QStyle::SP_DialogResetButton);

	// 设置按钮大小
	setMinimumSize(30, 30);
	setMaximumSize(30,30);
}

void ColorPickerButton::setCurrentColor(const QColor& color)
{
	if (m_currentColor != color) {
		m_currentColor = color;
		update(); // 触发重绘
		emit colorChanged(color);
	}
}

QColor ColorPickerButton::currentColor() const
{
	return m_currentColor;
}

void ColorPickerButton::paintEvent(QPaintEvent* event)
{
	Q_UNUSED(event);

	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);

	// 绘制按钮背景
	painter.fillRect(rect(), palette().button());

	// 绘制图标（上方）
	int iconSize = 15;
	QRect iconRect((width() - iconSize) / 2, 2, iconSize, iconSize);
	m_icon.paint(&painter, iconRect);

	// 绘制颜色条（下方）
	int colorBarHeight = 10;
	int colorBarWidth = 28;
	QRect colorBarRect((width() - colorBarWidth) / 2, iconSize+2,
		colorBarWidth, colorBarHeight);

	// 绘制颜色条边框
	painter.setPen(QPen(Qt::gray, 1));
	painter.drawRect(colorBarRect);

	// 填充颜色
	painter.fillRect(colorBarRect.adjusted(1, 1, -1, -1), m_currentColor);
}

void ColorPickerButton::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton) {
		// 弹出颜色选择对话框
		QColor selectedColor = QColorDialog::getColor(
			m_currentColor, this, tr("选择文本颜色")
		);

		// 如果用户选择了有效颜色
		if (selectedColor.isValid() && selectedColor != m_currentColor) {
			setCurrentColor(selectedColor);
		}
	}

	QToolButton::mousePressEvent(event);
}
