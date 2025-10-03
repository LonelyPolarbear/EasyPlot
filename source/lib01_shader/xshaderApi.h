#pragma once
#include <string>
#ifdef  LIB01_SHADER_DLL
#define lib01_shader_API __declspec(dllexport)
#else
#define lib01_shader_API __declspec(dllimport)
#endif //  lib01_shader_EXPORTS