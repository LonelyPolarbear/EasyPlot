#pragma once
#include <string>
#ifdef  LIB_STORAGE_DLL
#define lib_storage_API __declspec(dllexport)
#else
#define lib_storage_API __declspec(dllimport)
#endif 