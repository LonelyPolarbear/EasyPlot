#pragma once
#ifdef  LIBPANEL02_OPENGLWIDGET1_DLL	
#define lib02_openglwodget1_API __declspec(dllexport)
#else
#define lib02_openglwodget1_API __declspec(dllimport)
#endif 

#include <libPanle01_XOpenGLWidget/XOpenGLWidget.h>

class QOpenGLContext;
class QWindow;
/// <summary>
/// OpenGLWidget1，实现基本渲染流程，用于绘制一个矩形
/// </summary>
class lib02_openglwodget1_API OpenGLWidget1 : public XOpenGLWidget
{
	Q_OBJECT
public:
	explicit OpenGLWidget1(QWidget* parent);

	void virtual render() override;
	void virtual initGLResource();
};
