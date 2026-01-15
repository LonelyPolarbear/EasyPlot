#pragma once
#ifdef  XALGO_DLL
#define xalgo_API __declspec(dllexport)
#else
#define xalgo_API __declspec(dllimport)
#endif 