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

	int next_in_cycle(int x, int m, int n)
	{
		// 可选：确保输入合法（调试时有用）
		//assert(m <= n);
		//assert(x >= m && x <= n);

		int length = n - m + 1;                // 区间长度
		return m + (x - m + 1) % length;
	}

	std::optional<XQ::Vec3f> intersectLinePlane(const XQ::Vec3f& planePoint, const XQ::Vec3f& planeNormal, const XQ::Vec3f& linePoint, const XQ::Vec3f& lineDir)
	{
		std::optional<XQ::Vec3f> result;
		const double EPSILON = 1e-8; // 浮点数误差容忍度
		Eigen::Vector3f planePoint_(planePoint[0], planePoint[1], planePoint[2]);
		Eigen::Vector3f linePoint_(linePoint[0], linePoint[1], linePoint[2]);

		// 1. 计算分母：法线与直线方向的点乘
		Eigen::Vector3f lineDir_(lineDir[0], lineDir[1], lineDir[2]);
		Eigen::Vector3f planeNormal_(planeNormal[0], planeNormal[1], planeNormal[2]);

		planeNormal_.normalize();
		lineDir_.normalize();

		double denom = lineDir_.dot(planeNormal_);

		// 2. 判断是否平行
		if (std::abs(denom) < EPSILON) {
			return std::nullopt;
		}

		// 3. 计算参数 t
		Eigen::Vector3f diff = planePoint_ - linePoint_;
		double t = diff.dot(planeNormal_) / denom;

		// 4. 计算交点坐标
		Eigen::Vector3f intersection;
		intersection[0] = linePoint_[0] + t * lineDir_[0];
		intersection[1] = linePoint_[1] + t * lineDir_[1];
		intersection[2] = linePoint_[2] + t * lineDir_[2];

		result = XQ::Vec3f(intersection[0], intersection[1], intersection[2]);
		return result;
	}

	std::array< XQ::Vec3f, 4> getFrustumYOZIntersections(XQ::Vec3f near[4], /* near平面 [N1, N2, N3, N4] (按顺序) */ XQ::Vec3f far[4] /* far平面 [F1, F2, F3, F4] (按顺序) */)
	{
		std::array< XQ::Vec3f, 4> intersections;
		for (int i = 0; i < 4; i++) {
			Eigen::Vector3f pA = Eigen::Vector3f(far[i].x(), far[i].y(), far[i].z());
			Eigen::Vector3f pB = Eigen::Vector3f(near[i].x(), near[i].y(), near[i].z());
			Eigen::Vector3f m = Eigen::Vector3f(0, 0, 0);
			if (abs(pA.x() - pB.x())> 0.01) {
				m = pA + (pB - pA) * (-pA.x()) / (pB.x() - pA.x());            //交点   
			}
			intersections[i] = XQ::Vec3f(m[0], m[1], m[2]); // 左下
		}
		XQ::Vec3f minPt = XQ::Vec3f(0.0, 1e10, 1e10); // 初始化为极大值
		XQ::Vec3f maxPt = XQ::Vec3f(0.0, -1e10, -1e10); // 初始化为极小值
		for (int i = 0; i < 4; i++) {

			minPt = min(minPt, intersections[i]);
			minPt = min(minPt, intersections[i]);
			maxPt = max(maxPt, intersections[i]);
			maxPt = max(maxPt, intersections[i]);
		}
		intersections[0] = XQ::Vec3f(0,minPt.y(), minPt.z()); // 左下
		intersections[1] = XQ::Vec3f(0,maxPt.y(), minPt.z()); // 右下
		intersections[2] = XQ::Vec3f(0,maxPt.y(), maxPt.z()); // 右上
		intersections[3] = XQ::Vec3f(0,minPt.y(), maxPt.z()); // 左上
		return intersections;
	}

	std::array< XQ::Vec3f, 4> getFrustumXOYIntersections(XQ::Vec3f near[4], /* near平面 [N1, N2, N3, N4] (按顺序) */ XQ::Vec3f far[4] /* far平面 [F1, F2, F3, F4] (按顺序) */)
	{
		std::array< XQ::Vec3f, 4> intersections;
		for (int i = 0; i < 4; i++) {
			Eigen::Vector3f pA = Eigen::Vector3f(far[i].x(), far[i].y(), far[i].z());
			Eigen::Vector3f pB = Eigen::Vector3f(near[i].x(), near[i].y(), near[i].z());
			Eigen::Vector3f m = Eigen::Vector3f(0, 0, 0);
			if (abs(pA.z() - pB.z()) > 0.01) {
				m = pA + (pB - pA) * (-pA.z()) / (pB.z() - pA.z());            //交点   
			}
			intersections[i] = XQ::Vec3f(m[0], m[1], m[2]); // 左下
		}
		XQ::Vec3f minPt = XQ::Vec3f(1e10, 1e10, 0); // 初始化为极大值
		XQ::Vec3f maxPt = XQ::Vec3f(-1e10, -1e10, 0); // 初始化为极小值
		for (int i = 0; i < 4; i++) {

			minPt = min(minPt, intersections[i]);
			minPt = min(minPt, intersections[i]);
			maxPt = max(maxPt, intersections[i]);
			maxPt = max(maxPt, intersections[i]);
		}
		intersections[0] = XQ::Vec3f(minPt.x(), minPt.y(), 0); // 左下
		intersections[1] = XQ::Vec3f(maxPt.x(), minPt.y(), 0); // 右下
		intersections[2] = XQ::Vec3f(maxPt.x(), maxPt.y(), 0); // 右上
		intersections[3] = XQ::Vec3f(minPt.x(), maxPt.y(), 0); // 左上
		return intersections;

	}

	std::array< XQ::Vec3f, 4> getFrustumXOZIntersections(XQ::Vec3f near[4], XQ::Vec3f far[4])
	{
#if 1
		std::array< XQ::Vec3f, 4> intersections;
		for (int i = 0; i < 4; i++) {
			Eigen::Vector3f pA = Eigen::Vector3f(far[i].x(), far[i].y(), far[i].z());
			Eigen::Vector3f pB = Eigen::Vector3f(near[i].x(), near[i].y(), near[i].z());
			Eigen::Vector3f m = Eigen::Vector3f(0, 0, 0);
			if (abs(pA.y() - pB.y()) > 0.01) {
				m = pA + (pB - pA) * (-pA.y()) / (pB.y() - pA.y());            //交点   
			}
			intersections[i] = XQ::Vec3f(m[0], m[1], m[2]); // 左下
		}
		XQ::Vec3f minPt = XQ::Vec3f(1e10, 0.0, 1e10); // 初始化为极大值
		XQ::Vec3f maxPt = XQ::Vec3f(-1e10, 0.0, -1e10); // 初始化为极小值
		for (int i = 0; i < 4; i++) {

			minPt = min(minPt, intersections[i]);
			minPt = min(minPt, intersections[i]);
			maxPt = max(maxPt, intersections[i]);
			maxPt = max(maxPt, intersections[i]);
		}
		intersections[0] = XQ::Vec3f(minPt.x(), 0.0, minPt.z()); // 左下
		intersections[1] = XQ::Vec3f(maxPt.x(), 0.0, minPt.z()); // 右下
		intersections[2] = XQ::Vec3f(maxPt.x(), 0.0, maxPt.z()); // 右上
		intersections[3] = XQ::Vec3f(minPt.x(), 0.0, maxPt.z()); // 左上
		return intersections;

		return intersections;

#else
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
#endif
	}

	 float getInterpolationCoefficient(float a, float b, float c)
	{
		// 1. 防御性编程：如果起点和终点相同，直接返回 0.0 防止除零错误
		if (b == a) {
			return 0.0f;
		}

		float t = (c - a) / (b - a);
		if (t < 0.0f) t = 0.0f;
		if (t > 1.0f) t = 1.0f;
		return t;
	}
}