#pragma once
#ifdef  LIB05_SHAPE_DLL
#define LIB05_SHAPE_API __declspec(dllexport)
#else
#define LIB05_SHAPE_API __declspec(dllimport)
#endif 
