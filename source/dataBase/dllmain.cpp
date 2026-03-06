// dllmain.cpp.in
#include <windows.h>
#include <iostream>
#include "XObjectFactory.h"

BOOL APIENTRY DllMain(HMODULE hmod, DWORD reason, LPVOID reserved) {
    switch (reason) {
    case DLL_PROCESS_ATTACH:
		std::cout << "----------------------------------------------------" << std::endl;
		std::cout << "dataBase loaded" << std::endl;
		std::cout << "----------------------------------------------------" << std::endl;
        XBaseObjectMeta::InitializeCoreTypes();
        break;
    case DLL_PROCESS_DETACH:
        // cleanup if needed
        break;
    }
    return TRUE;
}
