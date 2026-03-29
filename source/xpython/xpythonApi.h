#pragma once
#ifdef  XPYTHON_DLL
#define XPYTHON_API __declspec(dllexport)
#else
#define XPYTHON_API __declspec(dllimport)
#endif 
