
#include <pybind11/pybind11.h>
#include <nlohmann/json.hpp>
#include <lib00_utilty/XUtilty.h>
#include <strstream>

namespace py = pybind11;
using json = nlohmann::json;

static void say_hello() {
	std::cout << "Hello, this is xdata!\n";
}

PYBIND11_MODULE(xdata, m) {
	m.def("hello_xdata", &say_hello, "say_hello");

} 

PY_INIT_HELPER(xdata)