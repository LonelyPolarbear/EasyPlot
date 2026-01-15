#pragma once
#include <dataBase/XClolor.h>
#include "XAlgoApi.h"
namespace XQ::XAlgo {
	extern xalgo_API XColor lerp_rgb(const XColor& c1, const XColor &c2, float t);

	struct xalgo_API XGridLayout{
		int rows =1;
		int cols =1;

		XGridLayout(int rows, int cols) : rows(rows), cols(cols) {}

		/// <summary>
		/// 左下角归一化坐标
		/// </summary>
		Vec4d getCellPos(int x, int y) const{
			auto xLen = 1.0f / cols;
			auto yLen = 1.0f / rows;
			auto leftCornerx = x*xLen;
			auto leftCornery = y*yLen;
			return Vec4d(leftCornerx, leftCornery, xLen, yLen);
		}
	};
}