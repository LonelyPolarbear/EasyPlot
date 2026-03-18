#include <filesystem>

#include <QApplication>
#include <QLoggingCategory>

#include <easyPlot/XEasyPlotWidget.h>

#include "mainwindow.h"
#include "XMainWindow.h"


int main(int argc, char** argv) {
	XQ::ShareVar::instance().currentExeDir = std::filesystem::path(argv[0]).parent_path().string();
	QApplication a(argc, argv);

	QLoggingCategory::setFilterRules("qt.qpa.window.setGeometry=false");
#if 0
	XEasyPlotWidget w(nullptr);
	w.resize(400, 300);
	w.show();
#else
	XMainWindow w(nullptr);
	w.resize(600, 400);
	w.show();
#endif
	return a.exec();
}