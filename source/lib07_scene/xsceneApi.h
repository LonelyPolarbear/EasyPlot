#pragma once
#include <string>
#ifdef LIB07_SCENE_DLL
#define LIB07_SCENE_API __declspec(dllexport)
#else
#define LIB07_SCENE_API __declspec(dllimport)
#endif 