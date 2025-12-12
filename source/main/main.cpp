#include <iostream>
#include <QApplication>
#include <filesystem>
#include <thread>
#include<lib00_utilty/myUtilty.h>
#include<dataBase/dataobject.h>
#include<libPanle03_OpenGLWidget2/OpenGLWidget2.h>
#include "mainwindow.h"

#include <QLoggingCategory>
#include <lib00_utilty/gp/XTraits.hpp>

#include <Eigen/Eigen>



int main(int argc,char** argv) {
#if 0
	//测试绕着任意轴旋转的变换

	Eigen::Affine3f frame = Eigen::Affine3f::Identity();
	frame.rotate(Eigen::AngleAxisf(1.2f, Eigen::Vector3f::UnitX()));
	frame.rotate(Eigen::AngleAxisf(0.7f, Eigen::Vector3f::UnitY()));

	Eigen::Vector3f ydir = frame*Eigen::Vector3f::UnitY();

	Eigen::Affine3f rot = Eigen::Affine3f::Identity();
	rot.rotate(Eigen::AngleAxisf(2, ydir));

	XQ::Matrix::dump(rot,std::cout);
	std::cout<<"\n";


	//
	{
		Eigen::Affine3f rot1 = Eigen::Affine3f::Identity();
		rot1.rotate(Eigen::AngleAxisf(2, Eigen::Vector3f::UnitY()));

		Eigen::Affine3f a = frame*rot1*frame.inverse();
		XQ::Matrix::dump(a, std::cout);
		std::cout << "\n";
	}

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