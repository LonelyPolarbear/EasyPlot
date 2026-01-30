#pragma once

#ifdef LIB02_CAMERA_DLL
#define lib02_CAMERA_API __declspec(dllexport)
#else
#define lib02_CAMERA_API __declspec(dllimport)
#endif // LIB02_SHADER_DLL