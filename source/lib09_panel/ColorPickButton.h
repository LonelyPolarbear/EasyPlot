#ifndef COLORPICKERBUTTON_H
#define COLORPICKERBUTTON_H

#include <QPushButton>
#include <QToolButton>
#include <QColor>
#include <QIcon>
#include "XPanleApi.h"

class lib09_XPANEL_API ColorPickerButton : public QToolButton
{
	Q_OBJECT
public:
	explicit ColorPickerButton(QWidget* parent = nullptr);

	// 设置当前颜色
	void setCurrentColor(const QColor& color);
	// 获取当前颜色
	QColor currentColor() const;

signals:
	// 颜色改变时发出信号
	void colorChanged(const QColor& color);

protected:
	// 重绘事件，用于绘制颜色条
	void paintEvent(QPaintEvent* event) override;
	// 点击事件，用于弹出颜色选择对话框
	void mousePressEvent(QMouseEvent* event) override;

private:
	QColor m_currentColor;  // 当前选中的颜色
	QIcon m_icon;          // 按钮图标
};

#endif // COLORPICKERBUTTON_H
