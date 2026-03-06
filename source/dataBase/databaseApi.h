#pragma once
#ifdef  DATABASE_DLL
#define DATABASE_API __declspec(dllexport)
#else
#define DATABASE_API __declspec(dllimport)
#endif 
