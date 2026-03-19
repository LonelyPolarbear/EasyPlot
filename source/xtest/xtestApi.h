#pragma once
#ifdef  XTEST_DLL
#define XTEST_API __declspec(dllexport)
#else
#define XTEST_API __declspec(dllimport)
#endif 
