// dllmain.cpp.in
#include <windows.h>
#include <iostream>
#include <xlog/XLogger.h>

BOOL APIENTRY DllMain(HMODULE hmod, DWORD reason, LPVOID reserved) {
    switch (reason) {
    case DLL_PROCESS_ATTACH:
        XLOG_DEBUG("DLL sharevar loaded");
        break;
    case DLL_PROCESS_DETACH:
        // cleanup if needed
        XLOG_DEBUG("DLL sharevar unload");
        break;
    }
    return TRUE;
}
