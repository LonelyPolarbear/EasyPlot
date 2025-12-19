#include <iostream>
#include <QApplication>
#include <filesystem>
#include <thread>
#include<lib00_utilty/myUtilty.h>
#include<dataBase/dataobject.h>
#include<dataBase/XDataArray.h>
#include<libPanle03_OpenGLWidget2/OpenGLWidget2.h>
#include "mainwindow.h"

#include <QLoggingCategory>
#include <lib00_utilty/gp/XTraits.hpp>

#include <Eigen/Eigen>

void test01() {
	{
		XQ::print("一维数组");

		auto tupleNum = 12;
		auto array1d = makeShareDbObject<XIntArray>();
		array1d->setComponent(1);
		array1d->setNumOfTuple(tupleNum);

		auto pdata = array1d->data(0);
		std::iota(pdata, pdata + tupleNum, 0);

		array1d->dump();

		XQ::print("一维数组转二维，组分相同");
		auto array2d = makeShareDbObject<XIntArray2D>();
		array2d->setComponent(1);
		array2d->setDimensions(4,3);
		array2d->memCopy(array1d);
		array2d->dump();

		XQ::print("一维数组转二维，组分不同");
		{
			auto array2d = makeShareDbObject<XIntArray2D>();
			array2d->setComponent(4);
			array2d->setDimensions(1, 4);
			array2d->memCopy(array1d);
			array2d->dump();
		}

		XQ::print("一维数组转二维，组分不同");
		{
			auto array2d = makeShareDbObject<XDataArray2D<XQ::Vec2i>>();
			array2d->setComponent(1);
			array2d->setDimensions(2, 4);
			array2d->memCopy(array1d);
			array2d->dump();
		}
	}
}

int main(int argc,char** argv) {
#if 0
	test01();
	return 0;

#else
	XQ::ShareVar::instance().currentExeDir = std::filesystem::path(argv[0]).parent_path().string();
	QApplication a(argc, argv);

	QLoggingCategory::setFilterRules("qt.qpa.window.setGeometry=false");
	
	MainWindow w(nullptr);
	w.resize(900,700);
	w.show();

	return a.exec();
#endif
}