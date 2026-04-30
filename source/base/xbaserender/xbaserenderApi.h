#pragma once
#ifdef  XBASERENDER_DLL
#define XBASERENDER_API __declspec(dllexport)
#else
#define XBASERENDER_API __declspec(dllimport)
#endif 
