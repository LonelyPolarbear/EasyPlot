#pragma once
#ifdef  LIB_EASYPLOT_DLL	
#define lib06_easyplot_API __declspec(dllexport)
#else
#define lib06_easyplot_API __declspec(dllimport)
#endif 

#include <XOpenGLWidget/XGLWidget.h>
#include <lib00_utilty/XUtilty.h>
#include <dataBase/XDataArray.h>

class lib06_easyplot_API XEasyPlotWidget : public XGLWidget
{
	Q_OBJECT
public:
	explicit XEasyPlotWidget(QWidget* parent);
	~XEasyPlotWidget();
};