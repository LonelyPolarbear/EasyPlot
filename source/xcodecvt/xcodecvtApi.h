#pragma once
#ifdef  XCODECVT_DLL
#define XCODECVT_API __declspec(dllexport)
#else
#define XCODECVT_API __declspec(dllimport)
#endif 
