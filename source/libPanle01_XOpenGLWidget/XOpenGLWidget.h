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
/// <summary>
/// XOpenGLWidget作为基类，用于提供OpenGL渲染功能
/// </summary>
class lib01_xopenglwodget_API XOpenGLWidget : public QWidget
{
	Q_OBJECT
public:
	explicit XOpenGLWidget(QWidget* parent);

	~XOpenGLWidget();
public:
	bool initOpenglContext();

	void virtual initGLResource(){}

	WId nativeHandle();

	bool makeCurrent();

	void doneCurrent();

	void swapBuffers();

	void resizeEvent(QResizeEvent* event) override;

	void timerEvent(QTimerEvent* event) override;

	void showEvent(QShowEvent* event) override;

	QPaintEngine* paintEngine()  const override;

	void virtual render();

	std::shared_ptr<XOpenGLContext> getContext() const;

	//将窗口坐标转换为左下角屏幕坐标系列
	myUtilty::Vec2u mapToGLScreen(const QPoint& point) const;

	myUtilty::Vec2f mapToNormGLScreen(const QPoint& point) const;
protected:
	bool m_isInit = false;
	int mWidth =0;
	int mHeight = 0;

	std::shared_ptr<XOpenGLContext> mContext;
};
