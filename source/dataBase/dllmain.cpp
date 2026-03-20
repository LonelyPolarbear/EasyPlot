// dllmain.cpp.in
#include <windows.h>
#include <iostream>
#include "XObjectFactory.h"
#include <xlog/XLogger.h>

BOOL APIENTRY DllMain(HMODULE hmod, DWORD reason, LPVOID reserved) {
    switch (reason) {
    case DLL_PROCESS_ATTACH:
		XLOG_TRACE("dataBase loaded");
        XBaseObjectMeta::InitializeCoreTypes();
        break;
    case DLL_PROCESS_DETACH:
        XLOG_TRACE("dataBase unLoaded");
        break;
    }
    return TRUE;
}
