#pragma once
#include <optional>
#include <array>
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

	/// <summary>
	/// 计算a b两点组成的线段与XOZ平面的交点
	/// </summary>
	extern xalgo_API std::optional<XQ::Vec3f> intersectSegmentWithXOZ(XQ::Vec3f a, XQ::Vec3f b);

	extern xalgo_API std::array< XQ::Vec3f,4> getFrustumXOZIntersections(XQ::Vec3f near[4],		// near平面 [N1, N2, N3, N4] (按顺序)
																															XQ::Vec3f far[4]			// far平面 [F1, F2, F3, F4] (按顺序)
																														);
}