#pragma once
#ifdef  DATABASE_DLL
#define database_API __declspec(dllexport)
#else
#define database_API __declspec(dllimport)
#endif 