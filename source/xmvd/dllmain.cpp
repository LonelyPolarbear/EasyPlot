// dllmain.cpp.in
#include <windows.h>
#include <iostream>
#include "Init.h"

BOOL APIENTRY DllMain(HMODULE hmod, DWORD reason, LPVOID reserved) {
    switch (reason) {
    case DLL_PROCESS_ATTACH:
        std::cout << "DLL xmvd loaded" << std::endl;
        XMVD_Init();
        break;
    case DLL_PROCESS_DETACH:
        // cleanup if needed
        std::cout << "DLL xmvd unloaded" << std::endl;
        break;
    }
    return TRUE;
}
