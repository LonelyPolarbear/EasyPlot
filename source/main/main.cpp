#include <filesystem>

#include <QApplication>
#include <QLoggingCategory>

#include <easyPlot/XEasyPlotWidget.h>

#include "mainwindow.h"


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