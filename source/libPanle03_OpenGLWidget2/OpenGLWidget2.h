#pragma once
#ifdef  LIBPANEL03_OPENGLWIDGET2_DLL	
#define lib03_openglwodget2_API __declspec(dllexport)
#else
#define lib03_openglwodget2_API __declspec(dllimport)
#endif 

#include <libPanle01_XOpenGLWidget/XOpenGLWidget.h>

class QOpenGLContext;
class QWindow;
class QMouseEvent;
class QWheelEvent;

/// <summary>
/// OpenGLWidget2，加入相机交互
/// </summary>
class lib03_openglwodget2_API OpenGLWidget2 : public XOpenGLWidget
{
	Q_OBJECT
public:
	explicit OpenGLWidget2(QWidget* parent);
	~OpenGLWidget2();

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
