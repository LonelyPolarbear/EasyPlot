
#include <crtdbg.h> 
#include <pybind11/pybind11.h>
#include <iostream>

static int add(int a, int b) {
	std::cout << "add(" << a << ", " << b << ")" << std::endl;
	return a + b;
}

namespace py = pybind11;
PYBIND11_MODULE(xrender, m) {
	m.def("add", &add, "Add two integers together");
	//py::class_<MyClass>(m, "MyClass")
		//.def(py::init<>())													// 默认构造函数
		//.def(py::init<int, double>())									// 带参数构造函数
		//.def(py::init<const std::string&>());						// 带引用参数
} 

class PyInitHelper_xrender
{
public:
	PyInitHelper_xrender()
	{
		PyImport_AppendInittab("xrender", &PyInit_xrender); \
	}
};
static PyInitHelper_xrender initHelperrswj; 