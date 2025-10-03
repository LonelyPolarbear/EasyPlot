#pragma once
#include <string>
#ifdef LIB04_OPENGL_DLL
#define LIB04_OPENGL_API __declspec(dllexport)
#else
#define LIB04_OPENGL_API __declspec(dllimport)
#endif 