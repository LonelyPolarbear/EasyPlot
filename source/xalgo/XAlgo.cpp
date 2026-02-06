#include "XAlgo.h"
#include "dataBase/XDataArray.h"
namespace XQ::XAlgo {
	XColor lerp_rgb(const XColor& c1, const XColor& c2, float t)
	{
		return XColor();
	}

	XQ::Vec3f min(XQ::Vec3f a, XQ::Vec3f b) {
		return XQ::Vec3f(std::min(a[0],b[0]), std::min(a[1], b[1]), std::min(a[2], b[2]));
	}

	XQ::Vec3f max(XQ::Vec3f a, XQ::Vec3f b) {
		return XQ::Vec3f(std::max(a[0], b[0]), std::max(a[1], b[1]), std::max(a[2], b[2]));
	}

	extern std::optional<XQ::Vec3f> intersectSegmentWithXOZ(XQ::Vec3f a, XQ::Vec3f b)
	{
		std::optional<XQ::Vec3f> out = std::nullopt;
		XQ::Vec3f p;
		float ya = a.y();
		float yb = b.y();
		const float EPS = 1e-6;

		// 如果两点都在Y=0附近，返回a
		if (abs(ya) < EPS && abs(yb) < EPS) {
			p = a;
			out = p;
			return out;
		}

		// 如果线段跨越Y=0（y_a * y_b <= 0）
		if (ya * yb <= 0.0) {
			float t = -ya / (yb - ya); // 参数t (0≤t≤1)
			if (t >= 0.0 && t <= 1.0) {
				p = a + t * (b - a);
				out = p;
				return out;
			}
		}
		return out;
	}

	std::array< XQ::Vec3f, 4> getFrustumXOZIntersections(XQ::Vec3f near[4], XQ::Vec3f far[4])
	{
		std::array< XQ::Vec3f, 4> intersections;
		// 步骤1：收集所有交点（最多12个）
		XQ::Vec3f points[12];
		int count = 0;

		// 1. 检查near平面的4条边
		for (int i = 0; i < 4; i++) {
			int j = (i + 1) % 4; // 闭环：N4-N1
			XQ::Vec3f p;
			if (auto p = intersectSegmentWithXOZ(near[i], near[j])) {
				points[count++] = *p;
			}
		}

		// 2. 检查far平面的4条边
		for (int i = 0; i < 4; i++) {
			int j = (i + 1) % 4; // 闭环：F4-F1
			XQ::Vec3f p;
			if (auto p = intersectSegmentWithXOZ(far[i], far[j])) {
				points[count++] = *p;
			}
		}

		// 3. 检查4条侧棱
		for (int i = 0; i < 4; i++) {
			XQ::Vec3f p;
			if (auto p = intersectSegmentWithXOZ(near[i], far[i])) {
				points[count++] = *p;
			}
		}

		// 步骤2：计算包围盒（轴对齐矩形）
		XQ::Vec3f minPt = XQ::Vec3f(1e10, 0.0, 1e10); // 初始化为极大值
		XQ::Vec3f maxPt = XQ::Vec3f(-1e10, 0.0, -1e10); // 初始化为极小值

		// 如果有交点，用交点计算包围盒
		if (count > 0) {
			for (int i = 0; i < count; i++) {
				minPt = min(minPt, points[i]);
				maxPt = max(maxPt, points[i]);
			}
		}
		// 如果没有交点，用所有顶点在Y=0的投影计算包围盒
		else {
			for (int i = 0; i < 4; i++) {
				XQ::Vec3f projNear = XQ::Vec3f(near[i].x(), 0.0, near[i].z());
				XQ::Vec3f projFar = XQ::Vec3f(far[i].x(), 0.0, far[i].z());
				minPt = min(minPt, projNear);
				minPt = min(minPt, projFar);
				maxPt = max(maxPt, projNear);
				maxPt = max(maxPt, projFar);
			}
		}

		// 步骤3：构建轴对齐四边形
		intersections[0] = XQ::Vec3f(minPt.x(), 0.0, minPt.z()); // 左下
		intersections[1] = XQ::Vec3f(maxPt.x(), 0.0, minPt.z()); // 右下
		intersections[2] = XQ::Vec3f(maxPt.x(), 0.0, maxPt.z()); // 右上
		intersections[3] = XQ::Vec3f(minPt.x(), 0.0, maxPt.z()); // 左上
		return intersections;
	}

	extern xalgo_API int next_in_cycle(int x, int m, int n)
	{
		// 可选：确保输入合法（调试时有用）
		//assert(m <= n);
		//assert(x >= m && x <= n);

		int length = n - m + 1;                // 区间长度
		return m + (x - m + 1) % length;
	}

	
}