#ifndef CUSTOMTOOLBUTTON_H
#define CUSTOMTOOLBUTTON_H

#include <QToolButton>
#include <QMouseEvent>
#include "XPanleApi.h"

class lib09_XPANEL_API CustomToolButton : public QToolButton
{
	Q_OBJECT
public:
	explicit CustomToolButton(QWidget* parent = nullptr) : QToolButton(parent) {
		// 允许设置checked状态（必须开启）
		setCheckable(true);
	}

protected:
	// 重写鼠标按下事件：忽略用户点击对状态的影响
	void mousePressEvent(QMouseEvent* event) override {
		// 如果按钮有菜单，仍允许点击弹出菜单（可选逻辑）
		if (menu()) {
			showMenu(); // 手动显示菜单
		}
		// 不调用父类实现，阻止状态切换
		event->accept();
	}

	// 重写鼠标释放事件：同样不触发状态变化
	void mouseReleaseEvent(QMouseEvent* event) override {
		event->accept(); // 不调用父类，阻止状态切换
	}

	// 重写键盘事件：忽略空格键等按键对状态的影响
	void keyPressEvent(QKeyEvent* event) override {
		// 屏蔽空格键（默认会切换checked状态）
		if (event->key() == Qt::Key_Space) {
			event->accept();
			return;
		}
		// 其他按键（如方向键）可正常处理（可选）
		QToolButton::keyPressEvent(event);
	}
};

#endif // CUSTOMTOOLBUTTON_H
