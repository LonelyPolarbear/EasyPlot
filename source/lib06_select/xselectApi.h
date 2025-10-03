#pragma once
#include <string>
#ifdef LIB06_SELECT_DLL
#define LIB06_SELECT_API __declspec(dllexport)
#else
#define LIB06_SELECT_API __declspec(dllimport)
#endif 