#pragma  once
#include "databaseApi.h"
#include <type_traits>
#include <functional>
#include <iostream>

namespace XQ {
	template<unsigned int N, typename T>
	struct Vector {
		T data[N]{ 0 };

		Vector() {}
		template<typename... Args, typename = std::enable_if_t<
			//XTraits::conjunction_v<std::decay_t<Args>...>
			std::conjunction_v<std::is_arithmetic<std::decay_t<Args>>...>
			>>
			Vector(Args&&... args) {
			setValues(std::forward<Args>(args)...);
		}

		T x() const { return data[0]; }
		T y() const { return data[1]; }
		T z() const { return data[2]; }
		T w()const { return data[3]; }

		T r() const { return data[0]; }
		T g() const { return data[1]; }
		T b() const { return data[2]; }
		T a()const { return data[3]; }

		T& x() { return data[0]; }
		T& y() { return data[1]; }
		T& z() { return data[2]; }
		T& w() { return data[3]; }

		T& r() { return data[0]; }
		T& g() { return data[1]; }
		T& b() { return data[2]; }
		T& a() { return data[3]; }

		template<typename... Args, typename = std::enable_if_t<
			std::conjunction_v<std::is_arithmetic<std::decay_t<Args>>...>
			>>
			void setValues(Args&&... args) {
			setTuple(std::forward_as_tuple(std::forward<Args>(args)...));
		}

		T& operator[](unsigned int i) {
			return data[i];
		}

		T* ptr(unsigned int i) {
			return &data[i];
		}

		const T* ptr(unsigned int i) const {
			return &data[i];
		}

		const T& operator[](unsigned int i)const {
			return data[i];
		}

		template<unsigned int N2, typename U, typename = std::enable_if_t< std::is_arithmetic_v<U>>>
		Vector(const Vector<N2, U>& v) {
			constexpr int num = XTraits::min_value(N, N2);
			setValuesImpl(std::make_index_sequence<num>{}, v.data);
		}

		template<unsigned int N2, typename U, typename = std::enable_if_t< std::is_arithmetic_v<U>>>
		Vector<N, T>& operator=(const Vector<N2, U>& v) {
			constexpr int num = std::min(N, N2);
			setValuesImpl(std::make_index_sequence<num>{}, v.data)
				return *this;
		}

		//操作符重载 +=
		template<typename U, typename = std::enable_if_t< std::is_arithmetic_v<U>>>
		Vector<N, T>& operator+=(const Vector<N, U>& v) {
			auto add = [](T& t, const U& v) {t += v; };
			foldFn<T, U>(add, data, v.data, std::make_index_sequence<N>{});
			return *this;
		}

		//操作符重载 -=
		template<typename U, typename = std::enable_if_t< std::is_arithmetic_v<U>>>
		Vector& operator-=(const Vector<N, U>& v) {
			auto subtract = [](T& t, const U& v) {t -= v; };
			foldFn<T, U>(subtract, data, v.data, std::make_index_sequence<N>{});
			return *this;
		}

		//操作符重载 *=
		template<typename U, typename = std::enable_if_t< std::is_arithmetic_v<U>>>
		Vector& operator*=(U scalar) {
			auto multiplicat = [](T& t, const U& v) {t *= v; };
			foldFn<T, U>(multiplicat, data, scalar, std::make_index_sequence<N>{});
			return *this;
		}

		//操作符重载 /=
		template<typename U, typename = std::enable_if_t< std::is_arithmetic_v<U>>>
		Vector& operator/=(U scalar) {
			auto division = [](T& t, const U& v) {t /= v; };
			foldFn<T, U>(division, data, scalar, std::make_index_sequence<N>{});
			return *this;
		}

		//全局*
		template<typename U>
		friend Vector<N, T> operator*(U scalar, const Vector<N, T>& c);

		//全局*
		template<typename U>
		friend Vector<N, T> operator*(const Vector<N, T>& c, U scalar);

		//全局/
		template<typename U>
		friend Vector<N, T> operator/(const Vector<N, T>& c, U scalar);

		//全局+
		template<typename U>
		friend Vector<N, decltype(std::declval<T>() + std::declval<U>())> operator+(const Vector<N, T>& a1, const Vector<N, U>& a2);

		//全局-
		template<typename U>
		friend Vector<N, decltype(std::declval<T>() - std::declval<U>())> operator-(const Vector<N, T>& a1, const Vector<N, U>& a2);

		friend std::ostream& operator<<(std::ostream& os, const Vector<N, T>& obj) {
			os << "(";
			for (int i = 0; i < N; i++) {
				if constexpr (std::is_same_v<T, char> || std::is_same_v<T, unsigned char>) {
					if (i == 0) {
						os << (int)obj.data[i];
					}
					else
						os << "," << (int)obj.data[i];
				}
				else {
					if (i == 0) {
						os << obj.data[i];
					}
					else
						os << "," << obj.data[i];
				}

			}
			os << ")";
			return os;
		}

	private:
		template< typename Tuple>
		void setTuple(Tuple&& args) {
			constexpr int num = std::tuple_size_v<Tuple>;
			if constexpr (N >= num) {
				setValuesImpl(std::make_index_sequence<num>{}, std::forward<Tuple>(args));
			}
			else {
				setValuesImpl(std::make_index_sequence<N>{}, std::forward<Tuple>(args));
			}
		}

		template<size_t...Is, typename Tuple>
		void setValuesImpl(std::index_sequence<Is...>, Tuple&& args) {
			((data[Is] = std::get<Is>(std::forward<Tuple>(args))), ...);
		}

		template<size_t...Is, typename U, typename = std::enable_if_t< std::is_arithmetic_v<U>>>
		void setValuesImpl(std::index_sequence<Is...>, const U* ptr) {
			((data[Is] = ptr[Is]), ...);
		}

		template<typename U, typename V, size_t...Is>
		void foldFn(const std::function<void(U&, const V&)>& fn, U* dest, const V* src, std::index_sequence<Is...>) {
			((fn(dest[Is], src[Is])), ...);
		}

		template<typename U, typename V, size_t...Is>
		void foldFn(const std::function<void(U&, const V&)>& fn, U* dest, const V& scalar, std::index_sequence<Is...>) {
			((fn(dest[Is], scalar)), ...);
		}
	};

	//全局*
	template<unsigned int N, typename T, typename U, typename = std::enable_if_t< std::is_arithmetic_v<U>>>
	Vector<N, T> operator*(U scalar, const Vector<N, T>& c) {
		Vector<N, T> res = c;
		res *= scalar;
		return res;
	}

	//全局*
	template<unsigned int N, typename T, typename U, typename = std::enable_if_t< std::is_arithmetic_v<U>>>
	Vector<N, T> operator*(const Vector<N, T>& c, U scalar) {
		Vector<N, T> res = c;
		res *= scalar;
		return res;
	}

	//全局/
	template<unsigned int N, typename T, typename U, typename = std::enable_if_t< std::is_arithmetic_v<U>>>
	Vector<N, T> operator/(const Vector<N, T>& c, U scalar) {
		Vector<N, T> res = c;
		res /= scalar;
		return res;
	}

	//全局+
	template<unsigned int N, typename T, typename U, typename = std::enable_if_t< std::is_arithmetic_v<U>>>
	Vector<N, decltype(std::declval<T>() + std::declval<U>())> operator+(const Vector<N, T>& a1, const Vector<N, U>& a2) {
		Vector<N, decltype(std::declval<T>() + std::declval<U>())> res;
		for (int i = 0; i < N; i++) {
			res.data[i] = a1.data[i] + a2.data[i];
		}
		return res;
	}

	//全局-
	template<unsigned int N, typename T, typename U, typename = std::enable_if_t< std::is_arithmetic_v<U>>>
	Vector<N, decltype(std::declval<T>() - std::declval<U>())> operator-(const Vector<N, T>& a1, const Vector<N, U>& a2) {
		Vector<N, decltype(std::declval<T>() - std::declval<U>())> res;
		for (int i = 0; i < N; i++) {
			res.data[i] = a1.data[i] - a2.data[i];
		}
		return res;
	}

	extern template class database_API Vector<2, char>;
	extern template class database_API Vector<2, unsigned char>;
	extern template class database_API Vector<2, float>;
	extern template class database_API Vector<2, double>;
	extern template class database_API Vector<2, int>;
	extern template class database_API Vector<2, unsigned int>;

	extern template class database_API Vector<3, char>;
	extern template class database_API Vector<3, unsigned char>;
	extern template class database_API Vector<3, float>;
	extern template class database_API Vector<3, double>;
	extern template class database_API Vector<3, int>;
	extern template class database_API Vector<3, unsigned int>;

	extern template class database_API Vector<4, char>;
	extern template class database_API Vector<4, unsigned char>;
	extern template class database_API Vector<4, float>;
	extern template class database_API Vector<4, double>;
	extern template class database_API Vector<4, int>;
	extern template class database_API Vector<4, unsigned int>;

	using Vec2f = Vector<2, float>;
	using Vec2d = Vector<2, double>;
	using Vec2i = Vector<2, int>;
	using Vec2u = Vector<2, unsigned int>;
	using Vec2u8 = Vector<2, unsigned char>;
	using Vec2i8 = Vector<2, char>;

	using Vec3f = Vector<3, float>;
	using Vec3d = Vector<3, double>;
	using Vec3i = Vector<3, int>;
	using Vec3u = Vector<3, unsigned int>;
	using Vec3u8 = Vector<3, unsigned char>;
	using Vec3i8 = Vector<3, char>;

	using Vec4f = Vector<4, float>;
	using Vec4d = Vector<4, double>;
	using Vec4i = Vector<4, int>;
	using Vec4u = Vector<4, unsigned int>;
	using Vec4u8 = Vector<4, unsigned char>;
	using Vec4i8 = Vector<4, char>;

	//2D点的别名
	using Point2f = Vec2f;
	using Point2d = Vec2d;
	using Point2i = Vec2i;
	using Point2u = Vec2u;

	//3D点的别名
	using Point3f = Vec3f;
	using Point3d = Vec3d;
	using Point3i = Vec3i;
	using Point3u = Vec3u;

	//4D其次坐标的别名
	using Point4f = Vec4f;
	using Point4d = Vec4d;
	using Point4i = Vec4i;
	using Point4u = Vec4u;

	//Rect
	using Rectf = Vec4f;
	using Rectd = Vec4d;
	using Recti = Vec4i;
	using Rectu = Vec4u;
}

namespace XQ::math {
	/// <summary>
			/// 分割单位圆
			/// </summary>
			/// <param name="num">分割数量</param>
			/// <param name="angle">被分割的圆的度数，单位角度 起始为X正半轴</param>
			/// <returns></returns>
	extern std::vector<XQ::Vec2f> database_API circleSplit(int num, float angle);

	extern std::vector<XQ::Vec2f> database_API getRectCorner(XQ::Vec2f corner1, XQ::Vec2f corner2);
}