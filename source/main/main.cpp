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

#include <boost/signals2.hpp>
#include <xsignal/XSignal.h>

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

struct NoMutex {
	void lock() {}
	void unlock() {}
	bool try_lock() { return true; }
};

class foo {
public:
	XSIGNAL(void(int, int)) sig1;
	XSIGNAL(int(int, int)) sig2;

public:
	void slot1(int, int) {
		std::cout<<"slot1\n";
	}

	int slot2(int, int) {
		return 10;
		std::cout << "slot2\n";
	}
};
void test02() {
	foo obj;
	xsig::connect(&obj, &foo::sig1, &obj, &foo::slot1);
	xsig::connect(&obj, &foo::sig2, &obj, &foo::slot2);
	xsig::connect(&obj, &foo::sig1, [](int, int)->void {  std::cout << "lambda slot1\n"; });
	xsig::connect(&obj, &foo::sig2, [](int, int)->int {  std::cout << "lambda slot2\n"; return 20;});
	obj.sig1(1, 1);
	obj.sig2(1, 1);

	std::cout<<"\n";
	auto shareObj = std::make_shared<foo>();
	xsig::connect(shareObj, &foo::sig1, shareObj, &foo::slot1);
	xsig::connect(shareObj, &foo::sig1, [](int, int)->void {  std::cout << "lambda share slot1\n"; });
	xsig::connect(shareObj, &foo::sig2, shareObj, &foo::slot2);
	xsig::connect(shareObj, &foo::sig2, [](int, int)->int {  std::cout << "lambda share slot2\n"; return 20; });
	auto num = shareObj->sig1.connection_count();
	shareObj->sig1.xemit(10, 10);
	shareObj->sig2.xemit(10, 10);
}
int main(int argc,char** argv) {
#if 1
	//test01();
	test02();
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