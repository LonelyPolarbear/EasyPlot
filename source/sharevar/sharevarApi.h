#pragma once
#ifdef  SHAREVAR_DLL
#define SHAREVAR_API __declspec(dllexport)
#else
#define SHAREVAR_API __declspec(dllimport)
#endif 
