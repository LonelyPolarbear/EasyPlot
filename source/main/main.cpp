#include <iostream>
#include <QApplication>
#include <filesystem>
#include <thread>
#include<lib00_utilty/myUtilty.h>
#include<libPanle03_OpenGLWidget2/OpenGLWidget2.h>
#include "mainwindow.h"

#include <QLoggingCategory>
#include <regex>


int main(int argc,char** argv) {
	myUtilty::ShareVar::instance().currentExeDir = std::filesystem::path(argv[0]).parent_path().string();
	QApplication a(argc, argv);

	QLoggingCategory::setFilterRules("qt.qpa.window.setGeometry=false");
	
	MainWindow w(nullptr);
	w.resize(900,700);
	w.show();

	return a.exec();
}