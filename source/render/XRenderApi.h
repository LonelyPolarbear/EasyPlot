#pragma once
#include <string>
#ifdef LIB_XRENDER_DLL
#define LIB_XRENDER_API __declspec(dllexport)
#else
#define LIB_XRENDER_API __declspec(dllimport)
#endif 