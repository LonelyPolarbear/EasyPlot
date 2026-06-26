#pragma once
#include <optional>
#include <array>
#include <dataBase/XClolor.h>
#include <dataBase/XDataArray.h>
#include "XAlgoApi.h"
namespace XQ::XAlgo {
	template<typename T>
	std::vector<T> linspace(T a, T b, int n_points) {
		// n_points = N + 2，即总点数（含端点）
		if (n_points <= 1) {
			if (n_points == 1) return { a };
			else return {};
		}
		std::vector<T> result;
		result.reserve(n_points);
		for (int i = 0; i < n_points; ++i) {
			T t = static_cast<T>(i) / (n_points - 1); // t ∈ [0, 1]
			result.push_back(a + t * (b - a));
		}
		return result;
	}
	
	extern xalgo_API XColor lerp_rgb(const XColor& c1, const XColor &c2, float t);

	/**
 * @brief 根据已知端点 a, b 和目标值 c，计算线性插值系数 t
 * @param a 起始值
 * @param b 结束值
 * @param c 目标值
 * @return 插值系数 t (范围通常在 0.0 ~ 1.0 之间)
 *         如果 a == b，则返回 0.0 以避免除零错误
 */
	extern xalgo_API float getInterpolationCoefficient(float a, float b, float c);

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
	extern xalgo_API std::array< XQ::Vec3f, 4> getFrustumYOZIntersections(XQ::Vec3f near[4],		// near平面 [N1, N2, N3, N4] (按顺序)
																																								XQ::Vec3f far[4]			// far平面 [F1, F2, F3, F4] (按顺序)
	);

	extern xalgo_API std::array< XQ::Vec3f, 4> getFrustumXOYIntersections(XQ::Vec3f near[4],		// near平面 [N1, N2, N3, N4] (按顺序)
																															XQ::Vec3f far[4]			// far平面 [F1, F2, F3, F4] (按顺序)
	);

	/**
	 * @brief 线面求交
	 */
	extern std::optional<XQ::Vec3f> intersectLinePlane(const XQ::Vec3f& planePoint, const  XQ::Vec3f& planeNormal,
		const XQ::Vec3f& linePoint, const XQ::Vec3f& lineDir);

	extern xalgo_API int next_in_cycle(int x, int m, int n) ;

	/**
	 * @brief 生成正弦波曲线
	 * @param T 正弦曲线的周期 
	 * @param numInT 每个周期包含的点的个数
	 * @param amplitude 幅值
	 * @param yoffset 垂直方向的偏移
	 * @param xoffset 垂直方向的偏移
	 * @param numOfT 生成几个周期的数据
	 */
	extern xalgo_API  sptr<XFloatArray> generateSinSave(double T,int numInT ,double amplitude,double xoffset, double yoffset,double numOfT);

	/// <summary>
	///  把做个组分相同的XDataArray合并为一个更大的XDataArray
	/// </summary>
	template<typename T>
	sptr <XDataArray1D<T>> combineArray(std::vector<sptr< XDataArray1D<T>>> datas) {
		auto totalTupleNum = 0;
		auto component = 1;
		std::vector<uint32_t> tupleOffset;
		tupleOffset.push_back(0);
		for (auto d : datas) {
			totalTupleNum += d->getNumOfTuple();
			tupleOffset.push_back(totalTupleNum);

			component = d->getComponent();
		}

		auto out = makeShareDbObject< XDataArray1D<T>>();
		out->setComponent(component);
		out->setNumOfTuple(totalTupleNum);

		for (int i = 0; i < datas.size(); i++) {
			auto pcoord = datas[i]->data(0);
			auto startTupleIdx = tupleOffset[i];
			auto tupleNum = tupleOffset[i + 1] - tupleOffset[i];
			out->memCopy(pcoord, tupleNum*component, startTupleIdx);
		}
		return out;

	}

	template<typename T>
	void combineArray(std::vector<sptr< XDataArray1D<T>>> datas, sptr< XDataArray1D<T>> out) {
		auto totalTupleNum = 0;
		auto component = 1;
		std::vector<uint32_t> tupleOffset;
		tupleOffset.push_back(0);
		for (auto d : datas) {
			totalTupleNum += d->getNumOfTuple();
			tupleOffset.push_back(totalTupleNum);

			component = d->getComponent();
		}
		if(out->getNumOfTuple() != totalTupleNum )
			out->setNumOfTuple(totalTupleNum);

		for (int i = 0; i < datas.size(); i++) {
			auto pcoord = datas[i]->data(0);
			auto startTupleIdx = tupleOffset[i];
			auto tupleNum = tupleOffset[i + 1] - tupleOffset[i];
			out->memCopy(pcoord, tupleNum * component, startTupleIdx);
		}
	}
}