#pragma once
#ifdef  XMVD_DLL
#define XMVD_API __declspec(dllexport)
#else
#define XMVD_API __declspec(dllimport)
#endif 
