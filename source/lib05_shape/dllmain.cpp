// dllmain.cpp.in
#include <windows.h>
#include <iostream>
#include "Init.h"

BOOL APIENTRY DllMain(HMODULE hmod, DWORD reason, LPVOID reserved) {
    switch (reason) {
    case DLL_PROCESS_ATTACH:
		std::cout << "----------------------------------------------------" << std::endl;
		std::cout << "lib05_shape loaded" << std::endl;
		std::cout << "----------------------------------------------------" << std::endl;
        InitRenderNode();
        break;
    case DLL_PROCESS_DETACH:
        // cleanup if needed
        break;
    }
    return TRUE;
}
