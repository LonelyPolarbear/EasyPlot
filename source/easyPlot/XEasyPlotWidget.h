#pragma once
#ifdef  LIB_EASYPLOT_DLL	
#define lib06_easyplot_API __declspec(dllexport)
#else
#define lib06_easyplot_API __declspec(dllimport)
#endif 

#include <XOpenGLWidget/XGLWidget.h>
#include <lib00_utilty/XUtilty.h>
#include <dataBase/XDataArray.h>
#include <xsignal/XSignal.h>
class XRender;
class lib06_easyplot_API XEasyPlotWidget : public XGLWidget
{
	Q_OBJECT
public:
	explicit XEasyPlotWidget(QWidget* parent);
	~XEasyPlotWidget();

	virtual void contextMenuEvent(QContextMenuEvent* event)override;

	void test1();
	void test2();
	void test3();
	void test4();


	void slotAddCube();
	void slotAddSphere();
	void slotAddCylinder();
	void slotAddChart();
	void slotAddRing();
	void slotAddCone();

	void initView();
public slots:
	void slotFitView3D();
};