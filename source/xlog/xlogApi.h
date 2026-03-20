#pragma once
#ifdef  XLOG_DLL
#define XLOG_API __declspec(dllexport)
#else
#define XLOG_API __declspec(dllimport)
#endif 
