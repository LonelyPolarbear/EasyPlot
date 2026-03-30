#pragma execution_character_set("utf-8")
#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/python.hpp>

using namespace boost::python;
#include <pybind11/embed.h>
#include <filesystem>

#include <QApplication>
#include <QLoggingCategory>

#include <easyPlot/XEasyPlotWidget.h>

//#include "mainwindow.h"
#include "XMainWindow.h"
#include <xpython/XPython.h>
#include <xpython/XPythonConsole.h>
#include <QTimer>
#include "lib00_utilty/gp/XTraits2.hpp"


int main(int argc, char** argv) {
	XQ::ShareVar::instance().currentExeDir = std::filesystem::path(argv[0]).parent_path().string();
	QApplication a(argc, argv);

	QLoggingCategory::setFilterRules("qt.qpa.window.setGeometry=false");

	XMainWindow w(nullptr);
	w.resize(600, 400);
	w.show();
	w.hide();

	auto ins = XPython::Instance();
	ins->execute("import xjson");
	ins->execute("import xdata");
	ins->execute("xjson.hello()");
	ins->execute("xdata.hello()");

	//构造时：调用 PyEval_SaveThread() 释放 GIL，并保存当前线程状态
	//析构时：调用 PyEval_RestoreThread() 重新获取 GIL。
	pybind11::gil_scoped_release release;

	XPythonConsole console;
	console.run();

	/*QTimer timer;
	timer.setInterval(20000);
	QObject::connect(&timer, &QTimer::timeout, [&](){
		ins->execute("print(11)");
	});
	timer.start();*/

	int ret = a.exec();
	console.stop();
	XPython::release();
	return ret;
}