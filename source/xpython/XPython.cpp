#include "XPython.h"
#include <pybind11/embed.h>
#include <xlog/XLogger.h>
static std::shared_ptr<XPython> s_instance;
class XPython::Internal {
public:
	Internal() {
		Py_SetPythonHome(L"E:/work/EasyPlot/Python38");
		global_interpreter = new pybind11::scoped_interpreter();

		std::string code =
			"import sys\n"
			"sys.path.insert(0, r'E:/work/EasyPlot/output/bin')\n"
			"import os\n"
			"if hasattr(os, 'add_dll_directory'):\n"
			"    os.add_dll_directory(r'E:/work/EasyPlot/output/bin')\n";
		//pybind11::exec(code);
		try {
			pybind11::exec(code);
		}
		catch (const pybind11::error_already_set& e) {
			PyErr_Print();
			XLOG_ERROR("XPython execute error: {}", code);
		}
	}

	~Internal() {
		delete global_interpreter;
	}

	pybind11::scoped_interpreter *global_interpreter = nullptr;
};
std::shared_ptr<XPython> XPython::Instance()
{
	if (!s_instance) {
		s_instance = makeShareDbObject<XPython>();
	}
	 
	return s_instance;
}

void XPython::release()
{
	s_instance.reset();
}

void XPython::execute(const std::string& code)
{
	pybind11::gil_scoped_acquire acquire;
	try {
		pybind11::exec(code);
	}
	catch (const pybind11::error_already_set& e) {
		PyErr_Print();
		XLOG_ERROR("XPython execute error: {}", code);
	}
	
}

XPython::XPython():mData(new Internal)
{

}
