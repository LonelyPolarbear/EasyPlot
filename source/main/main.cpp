#include <iostream>
#include <QApplication>
#include <filesystem>
#include <thread>
#include<lib00_utilty/myUtilty.h>
#include<libPanle03_OpenGLWidget2/OpenGLWidget2.h>
#include "mainwindow.h"

#include <regex>

std::vector<std::string> split(const std::string& input, std::regex& pattern) {
	std::vector<std::string> result;
	std::smatch matches;
	if (std::regex_match(input, matches, pattern)) {
		for (int i = 0; i < matches.size(); i++) {
			result.push_back(matches[i]);
		}
	}
	return result;
}

int main(int argc,char** argv) {

	myUtilty::ShareVar::instance().currentExeDir = std::filesystem::path(argv[0]).parent_path().string();
	QApplication a(argc, argv);
	
	MainWindow w(nullptr);
	w.resize(700,500);
	//OpenGLWidget2 w2(nullptr);
	//w2.show();
	w.show();

	return a.exec();
}