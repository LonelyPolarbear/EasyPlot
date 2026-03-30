// dllmain.cpp.in
#include <windows.h>
#include <iostream>
#include <xlog/XLogger.h>


BOOL APIENTRY DllMain(HMODULE hmod, DWORD reason, LPVOID reserved) {
    switch (reason) {
    case DLL_PROCESS_ATTACH:
        XLOG_TRACE("DLL xpython loaded");
        break;
    case DLL_PROCESS_DETACH:
        // cleanup if needed
        break;
    }
    return TRUE;
}
