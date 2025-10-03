#pragma once
#ifdef  LIBPANEL04_OPENGLWIDGET_CUBE_DLL	
#define lib04_openglwodget_API __declspec(dllexport)
#else
#define lib04_openglwodget_API __declspec(dllimport)
#endif 

#include <libPanle01_XOpenGLWidget/XOpenGLWidget.h>

class QOpenGLContext;
class QWindow;
class QMouseEvent;
class QWheelEvent;

/// <summary>
/// OpenGLWidgetCube，绘制一个立方体+相机交互
/// </summary>
class lib04_openglwodget_API OpenGLWidgetCube : public XOpenGLWidget
{
	Q_OBJECT
public:
	explicit OpenGLWidgetCube(QWidget* parent);
	~OpenGLWidgetCube();

	void virtual render() override;
	void virtual initGLResource();

protected:
	virtual void mousePressEvent(QMouseEvent* event) override;
	virtual void mouseMoveEvent(QMouseEvent* event) override;
	virtual void mouseReleaseEvent(QMouseEvent* event) override;
	virtual void wheelEvent(QWheelEvent* event) override;
	void resizeEvent(QResizeEvent* event) override;
private:
	struct Internal;
	std::unique_ptr<Internal> d;
};
