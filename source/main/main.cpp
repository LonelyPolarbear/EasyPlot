#include <iostream>
#include <QApplication>
#include <filesystem>
#include <thread>
#include<lib00_utilty/XUtilty.h>
#include<dataBase/dataobject.h>

#include<XOpenGLWidget/XGLWidget.h>
#include<XOpenGLWidget/XOpenGLWidget.h>
#include <easyPlot/XEasyPlotWidget.h>
#include "mainwindow.h"

#include <QLoggingCategory>
#include <lib00_utilty/gp/XTraits.hpp>

#include <Eigen/Eigen>

#include <boost/signals2.hpp>
#include <xsignal/XSignal.h>


int main(int argc, char** argv) {
	XQ::ShareVar::instance().currentExeDir = std::filesystem::path(argv[0]).parent_path().string();
	QApplication a(argc, argv);

	QLoggingCategory::setFilterRules("qt.qpa.window.setGeometry=false");
#if 1
	XEasyPlotWidget w(nullptr);
	w.resize(400, 300);
	w.show();
#else
	MainWindow w(nullptr);
	w.resize(900, 700);
	w.show();
#endif
	return a.exec();
}