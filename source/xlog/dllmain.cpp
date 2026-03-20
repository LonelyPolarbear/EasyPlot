// dllmain.cpp.in
#include <windows.h>
#include <iostream>
#include "XLogger.h"

BOOL APIENTRY DllMain(HMODULE hmod, DWORD reason, LPVOID reserved) {
    switch (reason) {
    case DLL_PROCESS_ATTACH:
        //全局 / 静态对象的构造函数在 DLL 加载时（DllMain(DLL_PROCESS_ATTACH) 阶段）被调用；
        //通常是在 DllMain 执行之前，由 CRT（C Runtime）初始化代码触发
        XLOG_TRACE("====DLL xlog loaded====");

        break;
    case DLL_PROCESS_DETACH:
        // cleanup if needed
        //析构函数在 DLL 卸载时（DllMain(DLL_PROCESS_DETACH) 阶段）被调用
        //通常是在 DllMain 执行之后，由 CRT 清理代码触发
  
        XLOG_TRACE("====DLL xlog unload====");
        spdlog::shutdown();
        break;
    }
    return TRUE;
}
