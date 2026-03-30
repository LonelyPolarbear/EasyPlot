#include <pybind11/pybind11.h>
#include <pybind11/stl.h>     
#include <nlohmann/json.hpp>

namespace py = pybind11;
using json = nlohmann::json;
json pyobject_to_json(const py::object& obj) {
	if (obj.is_none()) {
		return json();
	}
	
	if (py::isinstance<py::bool_>(obj)) {
		return obj.cast<bool>();
	}
	if (py::isinstance<py::int_>(obj)) {
		return obj.cast<int64_t>();
	}
	if (py::isinstance<py::float_>(obj)) {
		return obj.cast<double>();
	}
	if (py::isinstance<py::str>(obj)) {
		return obj.cast<std::string>();
	}
	if (py::isinstance<py::list>(obj)) {
		json arr = json::array();
		for (auto item : obj.cast<py::list>()) {
			arr.push_back(pyobject_to_json(py::object(item,false)));
		}
		return arr;
	}
	if (py::isinstance<py::dict>(obj)) {
		json j_obj = json::object();
		for (auto item : obj.cast<py::dict>()) {
			std::string key = item.first.cast<std::string>();
			j_obj[key] = pyobject_to_json(py::object(item.second, false));
		}
		return j_obj;
	}
	// 如果是已经绑定的 json 对象，直接转换（可能通过拷贝）
	if (py::isinstance<json>(obj)) {
		return obj.cast<json>();
	}
	throw std::runtime_error("Unsupported Python type for json conversion");
}

py::object json_to_pyobject(const json& j) {
	if (j.is_null()) {
		return py::none();
	}
	if (j.is_boolean()) {
		return py::bool_(j.get<bool>());
	}
	if (j.is_number_integer()) {
		return py::int_(j.get<int64_t>());
	}
	if (j.is_number_float()) {
		return py::float_(j.get<double>());
	}
	if (j.is_string()) {
		return py::str(j.get<std::string>());
	}
	if (j.is_array()) {
		py::list lst;
		for (const auto& item : j) {
			lst.append(json_to_pyobject(item));
		}
		return lst;
	}
	if (j.is_object()) {
		py::dict d;
		for (auto& [key, val] : j.items()) {
			d[py::str(key)] = json_to_pyobject(val);
		}
		return d;
	}
	throw std::runtime_error("Unsupported JSON type for Python conversion");
}
