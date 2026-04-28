#pragma once
#ifdef  XRENDERNODE_DLL
#define XRENDERNODE_API __declspec(dllexport)
#else
#define XRENDERNODE_API __declspec(dllimport)
#endif 
