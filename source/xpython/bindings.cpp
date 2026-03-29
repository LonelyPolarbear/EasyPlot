#if 1
#include <crtdbg.h> 
#include <pybind11/pybind11.h>
#include <nlohmann/json.hpp>
#include <lib00_utilty/XUtilty.h>


namespace py = pybind11;
using json = nlohmann::json;
static void say_hello() {
	std::cout << "Hello, this is xjson!\n";
}

PYBIND11_MODULE(xjson, m) {
	m.def("hello", &say_hello, "say_hello");
	//py::class_<MyClass>(m, "MyClass")
		//.def(py::init<>())													// 默认构造函数
		//.def(py::init<int, double>())									// 带参数构造函数
		//.def(py::init<const std::string&>());						// 带引用参数

		// 绑定 json 类
	py::class_<json,std::shared_ptr<nlohmann::json>>(m, "json")
		// 构造函数
		.def(py::init<>())
		.def(py::init<const json&>())               // 拷贝构造
		.def(py::init<const char*>())               // 从字符串解析
		.def(py::init<const std::string&>())
		// 序列化
		//.def("dump", &json::dump, py::arg("indent") = -1)
		.def("__str__", &json::dump)
		// 容器操作（使用 pybind11 自动将 json 转换为 Python 对象）
		.def("__getitem__", [](const json& j, const std::string& key) {
		return py::cast(j.at(key));
			})
		.def("__setitem__", [](json& j, const std::string& key, py::object value) {
		j[key] = value.cast<json>();
			})
		.def("__contains__", [](const json& j, const std::string& key) {
		return j.contains(key);
			})
		.def("get", [](const json& j, const std::string& key, py::object default_val = py::none()) -> py::object {
		auto it = j.find(key);
		if (it != j.end()) return py::cast(*it);
		return default_val;
			})
		// 静态方法：解析 JSON 字符串
		.def_static("parse", [](const std::string& s) {
		return json::parse(s);
			})
		// 类型判断（可选）
		.def("is_object", &json::is_object)
		.def("is_array", &json::is_array)
		.def("is_number", &json::is_number)
		.def("is_string", &json::is_string)
		.def("is_boolean", &json::is_boolean)
		.def("is_null", &json::is_null);
} 

PY_INIT_HELPER(xjson)
#endif