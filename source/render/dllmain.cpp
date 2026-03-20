// dllmain.cpp.in
#include <windows.h>
#include <iostream>
#include "Init.h"
#include <xlog/XLogger.h>

BOOL APIENTRY DllMain(HMODULE hmod, DWORD reason, LPVOID reserved) {
    switch (reason) {
    case DLL_PROCESS_ATTACH:
        XLOG_TRACE("render dll loaded");
        InitRender();
        break;
    case DLL_PROCESS_DETACH:
        // cleanup if needed
        XLOG_TRACE("render dll unload");
        break;
    }
    return TRUE;
}
