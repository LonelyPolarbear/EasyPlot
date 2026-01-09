#pragma once

#ifdef  LIBPANEL01_XOPENGLWIDGET_DLL	
#define lib01_xopenglwodget_API __declspec(dllexport)
#else
#define lib01_xopenglwodget_API __declspec(dllimport)
#endif 

#include <QWidget>
#include <lib00_utilty/myUtilty.h>

class QOpenGLContext;
class QWindow;
class QTimerEvent;
class QResizeEvent;
class QShowEvent;
class QPaintEngine;
class XOpenGLContext;
class XOpenGLRenderWindow;
class QMouseEvent;
class QKeyEvent;
class QFocusEvent;
/// <summary>
/// XOpenGLWidget作为基类，用于提供OpenGL渲染功能
/// </summary>
class lib01_xopenglwodget_API XGLWidget : public QWidget
{
	Q_OBJECT
public:
	explicit XGLWidget(QWidget* parent);

	~XGLWidget();
public:
	std::shared_ptr<XOpenGLRenderWindow> getRenderWindow();
	void renderTest();
protected:
	QPaintEngine* paintEngine()  const override;

	void virtual render();

	//将窗口坐标转换为左下角屏幕坐标系列
	XQ::Vec2u mapToGLScreen(const QPoint& point) const;

	XQ::Vec2f mapToNormGLScreen(const QPoint& point) const;

	bool initOpenglContext();
protected:
	//事件重写
	void timerEvent(QTimerEvent* event) override;
	void enterEvent(QEvent* event) override;
	void leaveEvent(QEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
	void keyPressEvent(QKeyEvent* event) override;
	void keyReleaseEvent(QKeyEvent* event) override;
	void focusInEvent(QFocusEvent* event) override;
	void focusOutEvent(QFocusEvent* event) override;
	void wheelEvent(QWheelEvent* event) override;
	void resizeEvent(QResizeEvent* event) override;
	void showEvent(QShowEvent* event) override;
protected:
	bool m_isInit = false;
	int mWidth =0;
	int mHeight = 0;

	std::shared_ptr<XOpenGLRenderWindow> mRenderWindow;
};
