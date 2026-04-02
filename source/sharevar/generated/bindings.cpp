#if 1
#include <crtdbg.h> 
#include <pybind11/pybind11.h>
#include <nlohmann/json.hpp>
#include <lib00_utilty/XUtilty.h>
#include "pythonTool.hpp"
#include <strstream>
#include "../XShareVar.h"

/*
* template <typename Type_, typename... Options>
class class_;
第一个参数 Type_：要绑定的 C++ 类型。

后续参数 Options...：可以包含：

0 个或多个基类（如果该类有继承关系）

0 个或 1 个 holder 类型（如 std::shared_ptr<Type_>）

其他标签（如 py::dynamic_attr() 通常作为构造函数参数，不是模板参数）

py::class_<Derived, Base, std::shared_ptr<Derived>,Options...>
py::class_<Derived, std::shared_ptr<Derived>,Options...>
(scope, name, doc,py::dynamic_attr())
)

*/
namespace py = pybind11;
using json = nlohmann::json;
//测试1 绑定静态函数
static void say_hello() {
	std::cout << "Hello, this is xjson!\n";
}

struct foo {
	int x;
	int y;
	int z;
	std::string dump() {
		std::stringstream osm;
		osm<<"x:"<<x<<",y:"<<y<<",z:"<<z;
		return osm.str();
	}
};


/*
__str__ 是 Python 的内置方法：当你在 Python 中调用 str(obj) 或 print(obj) 时，
Python 会自动调用对象的 __str__ 方法，期望返回一个字符串。通过 .def("__str__", ...)，
我们告诉 pybind11：这个 C++ 类的 Python 包装类应该实现 __str__ 方法，
其行为由我们提供的 C++ 函数决定。
*/
PYBIND11_MODULE(xjson, m) {
	m.def("hello_xjson", &say_hello, "say_hello");

	// 绑定 foo 类
	py::class_<foo>(m, "foo")
		.def(py::init<>())
		.def_readwrite("x", &foo::x)
		.def_readwrite("y", &foo::y)
		.def_readwrite("z", &foo::z)		//def_readonly
		.def("dump", &foo::dump);

	// 绑定 json 类
	py::class_<json,std::shared_ptr<nlohmann::json>>(m, "json")
		// 构造函数
		.def(py::init<>())
		.def(py::init<const json&>())               // 拷贝构造
		.def(py::init<const char*>())               // 从字符串解析
		.def(py::init([](const std::string& s) {             // 从 JSON 字符串解析
				return json::parse(s);
			}))
		// 序列化
		/*.def("dump", [](const json& j, int indent = -1, char indent_char = ' ') {
		return j.dump(indent, indent_char);
		})*/
		//str(obj) 或 print(obj) 时，Python 会调用 obj.__str__() //__str__默认啊绑定无参版本
		.def("__str__", [](const json& j){
			return j.dump();
		})
		//当使用 obj[key] 语法时，Python 会调用 obj.__getitem__(key)
		.def("__getitem__", [](const json& j, const std::string& key) {
				try {
					return json_to_pyobject(j.at(key));
				}
				catch (const std::out_of_range&) {
					throw py::key_error(key);
				}
			})
		//当使用 obj[key] = value 时，Python 会调用 obj.__setitem__(key, value)
		.def("__setitem__", [](json& j, const std::string& key, py::object value) {
		j[key] = pyobject_to_json(value);
			})
		//当使用 key in obj 时，Python 会调用 obj.__contains__(key)
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

PYBIND11_MODULE(sharevar, m) {
	py::class_<XShareVar, std::shared_ptr<XShareVar>>(m,"XShareVar")
		//.def(py::init<>())
		.def_static("instance", &XShareVar::instance)
		.def("getJson", &XShareVar::getJson, py::return_value_policy::reference);
}

PY_INIT_HELPER(sharevar)

#endif