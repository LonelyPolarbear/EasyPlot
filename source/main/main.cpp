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

int main(int argc, char** argv) {
	XQ::ShareVar::instance().currentExeDir = std::filesystem::path(argv[0]).parent_path().string();
	QApplication a(argc, argv);

	QLoggingCategory::setFilterRules("qt.qpa.window.setGeometry=false");

	XMainWindow w(nullptr);
	w.resize(600, 400);
	w.show();

	//调用python
	Py_SetPythonHome(L"E:/work/EasyPlot/Python38");
	pybind11::scoped_interpreter guard{};
	pybind11::exec(R"(
			import sys
			sys.path.insert(0, r'E:/work/EasyPlot/output/bin')
			import os
			if hasattr(os, 'add_dll_directory'):
			os.add_dll_directory(r'E:/work/EasyPlot/output/bin')
	)");

	// 导入 libpy 模块
	pybind11::module libpy = pybind11::module::import("xrender");

	pybind11::exec(R"(print(helloworld)");

	// 调用模块中的 add 函数
	int result = libpy.attr("add")(3, 5).cast<int>();
	std::cout << "Result: " << result << std::endl;

	return a.exec();
}