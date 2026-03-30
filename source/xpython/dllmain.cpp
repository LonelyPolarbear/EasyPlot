// dllmain.cpp.in
#include <windows.h>
#include <iostream>
#include <xlog/XLogger.h>
#include "xpython/XPython.h"

#include <pybind11/embed.h>
void XPythonInit() {
    //auto ins = XPython::Instance();
	//ins->execute("import xjson");
}

BOOL APIENTRY DllMain(HMODULE hmod, DWORD reason, LPVOID reserved) {
    switch (reason) {
    case DLL_PROCESS_ATTACH:
        XLOG_TRACE("DLL xpython loaded");
        XPythonInit();
        break;
    case DLL_PROCESS_DETACH:
        // cleanup if needed
        break;
    }
    return TRUE;
}
