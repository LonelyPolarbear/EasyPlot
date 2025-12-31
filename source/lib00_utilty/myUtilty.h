#ifndef __myutilty_hpp
#define __myutilty_hpp

#ifdef  LIB00_UTILTY_DLL
#define lib00_utilty_API __declspec(dllexport)
#else
#define lib00_utilty_API __declspec(dllimport)
#endif 

#include <string>
#include <Eigen/Eigen>
#include <iomanip>
#include <chrono>
#include <random>
#include <future>
#include <any>
namespace render {
	enum  class lib00_utilty_API graphicsItemType{
		none,
		rect,
		line,
		triangle,
		circle,
		ellipse,
		polyline,
		polygon,
		text
	};
}

namespace XQ {
	// 前置声明友元函数模板
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

		T x() const{ return data[0]; }
		T y() const{ return data[1]; }
		T z() const{ return data[2]; }
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

		const T& operator[](unsigned int i)const {
			return data[i];
		}

		template<unsigned int N2, typename U, typename = std::enable_if_t< std::is_arithmetic_v<U>>>
		Vector(const Vector<N2, U>& v) {
			constexpr int num =XTraits::min_value(N, N2);
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
			auto add=[](T & t, const U & v) {t+=v;};
			foldFn<T,U>(add,data,v.data,std::make_index_sequence<N>{});
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

		friend std::ostream& operator<<(std::ostream& os, const Vector<N, T> & obj) {
			os<<"(";
			for (int i = 0; i < N; i++) {
				if constexpr (std::is_same_v<T, char> || std::is_same_v<T, unsigned char>) {
					if (i == 0) {
						os << (int)obj.data[i];
					}
					else
						os << "," << (int)obj.data[i];
				}else{
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
	Vector<N, T> operator*( const Vector<N, T>& c, U scalar) {
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
			res.data[i] = a1.data[i]+ a2.data[i];
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

	extern template class lib00_utilty_API Vector<2, char>;
	extern template class lib00_utilty_API Vector<2, unsigned char>;
	extern template class lib00_utilty_API Vector<2, float>;
	extern template class lib00_utilty_API Vector<2, double>;
	extern template class lib00_utilty_API Vector<2, int>;
	extern template class lib00_utilty_API Vector<2, unsigned int>;

	extern template class lib00_utilty_API Vector<3, char>;
	extern template class lib00_utilty_API Vector<3, unsigned char>;
	extern template class lib00_utilty_API Vector<3, float>;
	extern template class lib00_utilty_API Vector<3, double>;
	extern template class lib00_utilty_API Vector<3, int>;
	extern template class lib00_utilty_API Vector<3, unsigned int>;

	extern template class lib00_utilty_API Vector<4, char>;
	extern template class lib00_utilty_API Vector<4, unsigned char>;
	extern template class lib00_utilty_API Vector<4, float>;
	extern template class lib00_utilty_API Vector<4, double>;
	extern template class lib00_utilty_API Vector<4, int>;
	extern template class lib00_utilty_API Vector<4, unsigned int>;

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
}

namespace XQ {
	extern lib00_utilty_API double PI;
	
	enum class tranformType {
		translate,
		rotateX,
		rotateY,
		rotateZ,
		scale
	};

	struct lib00_utilty_API transfomData
	{
		//获取等价的旋转顺序  T  RZ RX RY S
		float tx = 0;
		float ty = 0;
		float tz = 0;
		float rx = 0;
		float ry = 0;
		float rz = 0;
		float sx = 1;
		float sy = 1;
		float sz = 1;

		friend static std::ostream& operator<<(std::ostream& osm, transfomData data) {
			std::ostringstream stm;
			stm << std::fixed << std::setprecision(3)
				<< "transfomData{\n"
				<< " " << "float tx:" << data.tx << ";\n"
				<< " " << "float ty:" << data.ty << ";\n"
				<< " " << "float tz:" << data.tz << ";\n"
				<< " " << "float rx:" << data.rx << ";\n"
				<< " " << "float ry:" << data.ry << ";\n"
				<< " " << "float rz:" << data.rz << ";\n"
				<< " " << "float sx:" << data.sx << ";\n"
				<< " " << "float sy:" << data.sy << ";\n"
				<< " " << "float sz:" << data.sz << ";\n"
				<< "};" << std::endl;
			;
			osm << stm.str();
			return osm;
		}
	};

	struct lib00_utilty_API Matrix {
		inline static double PI = 3.14159265358979323846;

		static std::string dump(Eigen::Affine3f m, std::ostream& ostm) {
			return dump(m.matrix(), ostm);
		}

		static std::string dump(Eigen::Vector3f v, std::ostream& ostm) {
			std::stringstream stm;
			stm << std::fixed << std::setprecision(2) << v[0] << " " << v[1] << " " << v[2] << "\n";
			ostm << stm.str();
			return stm.str();
		}

		static std::string dump(Eigen::Matrix4f m, std::ostream& ostm) {
			std::stringstream stm;
			stm << std::fixed << std::setprecision(6);
			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 4; j++) {
					stm << m(i, j) << " ";
				}
				stm << "\n";
			}
			stm << "\n";
			ostm << stm.str();
			return stm.str();
		}

		static std::string dump(Eigen::Matrix3f m, std::ostream& ostm) {
			std::stringstream stm;
			stm << std::fixed << std::setprecision(2);
			for (int i = 0; i < 3; i++) {
				for (int j = 0; j < 3; j++) {
					stm << m(i, j) << " ";
				}
				stm << "\n";
			}
			stm << "\n";
			ostm << stm.str();
			return stm.str();
		}

		static std::string dump(Eigen::Quaternionf q, std::ostream& ostm) {
			std::stringstream stm;
			stm << std::fixed << std::setprecision(2);
			stm << "w:" << q.w() << " x:" << q.x() << " y:" << q.y() << " z:" << q.z();
			ostm << stm.str();
			return stm.str();
		}

		static Eigen::Matrix4f convert(Eigen::Matrix3f m) {
			Eigen::Matrix4f ret = Eigen::Matrix4f::Identity();
			ret.block(0, 0, 3, 3) = m.block(0, 0, 3, 3);
			//ret.topLeftCorner<3, 3>() = m;
			return ret;
		}

		static Eigen::Vector3f dir(Eigen::Matrix4f m, int type) {
			Eigen::Affine3f trans;
			trans.matrix() = m;
			if (type == 0) {
				return trans.rotation().block(0, 0, 3, 1);
			}
			if (type == 1) {
				return trans.rotation().block(0, 1, 3, 1);
			}
			if (type == 2) {
				return trans.rotation().block(0, 2, 3, 1);
			}
		}

		static Eigen::Vector3f translation(Eigen::Matrix4f m) {
			Eigen::Affine3f trans;
			trans.matrix() = m;
			return trans.translation();
		}

		static Eigen::Vector3f scale(Eigen::Matrix4f m) {
			//T S RZ RX RY
			Eigen::Affine3f trans;
			trans.matrix() = m;
			Eigen::Matrix3f scaleMatrix;
			Eigen::Matrix3f rotMatrix;
			trans.computeScalingRotation(&scaleMatrix, &rotMatrix);
			return Eigen::Vector3f(rotMatrix(0, 0), rotMatrix(1, 1), rotMatrix(2, 2));
		}

		/**
	   * @brief 角度转弧度
	   */
		static float radian(float v) {
			return v * PI / 180.0;
		}

		/**
		 * @brief 弧度转角度
		 */
		static float  Angle(float v) {
			return v * 180.0 / PI;
		}

		static Eigen::Affine3f scale(float sx, float sy, float sz) {
			Eigen::Affine3f t = Eigen::Affine3f::Identity();
			t.scale(Eigen::Vector3f(sx, sy, sz));
			return t;
		}

		static Eigen::Affine3f transltae(float tx, float ty, float tz) {
			Eigen::Affine3f t = Eigen::Affine3f::Identity();
			t.translation() << tx, ty, tz;
			return t;
		}

		/**
		 * 提取旋转矩阵
		 */
		static Eigen::Affine3f rotate(const Eigen::Affine3f& m) {
			Eigen::Affine3f t = Eigen::Affine3f::Identity();
			t.linear() = m.rotation();
			return t;
		}

		/**
		 * @brief
		 * @param angle 角度
		 */
		static Eigen::Affine3f rotate(float angle, Eigen::Vector3f dir) {
			Eigen::Affine3f t = Eigen::Affine3f::Identity();
			dir.normalize();
			t.rotate(Eigen::AngleAxisf(radian(angle), dir));
			return t;
		}

		static transfomData transformDecomposition_TRS(Eigen::Affine3f transform, std::vector<tranformType> eulerOrder = { 
																																													tranformType::rotateZ,
																																													tranformType::rotateX,
																																													tranformType::rotateY });

		static transfomData transformDecomposition_TSR(Eigen::Affine3f transform, std::vector<tranformType> eulerOrder = {
																																													tranformType::rotateZ,
																																													tranformType::rotateX,
																																													tranformType::rotateY });

		// 1->平移 2->rx 3->ry 4->rz 5->scale
		static Eigen::Matrix4f computeMatrix(transfomData data, std::vector<tranformType> order = { 
																																								tranformType::translate, 
																																								tranformType::rotateZ,
																																								tranformType::rotateX,
																																								tranformType::rotateY,
																																								tranformType::scale }
																																								);

		static Eigen::Matrix4f perspective(float fovy, float aspect, float znaer, float zfar);

		static Eigen::Matrix4f ortho(float left, float right, float bottom, float top, float zNear, float zFar);

		static float tb_project_to_sphere(float& sphereX, float& sphereY);

	};

	struct lib00_utilty_API ShareVar
	{
		static ShareVar& instance() {
			static ShareVar var;
			return var;
		}
		std::string currentExeDir;
	};
	
	struct lib00_utilty_API BoundBox {
		double xmin;
		double ymin;
		double zmin;
		double xmax;
		double ymax;
		double zmax;
	};


	namespace chrono {
		/// <summary>
		/// 时钟计时器，用于精确统计时间间隔
		/// </summary>
		struct lib00_utilty_API clock {
			void start();
			void restart();
			int64_t elapsed_ms();
			double elapsed_seconds();
		public:
			static clock& Instance() {
				static clock instance;
				return instance;
			}
		private:
			std::chrono::high_resolution_clock::time_point start_time_point;
		};
	}

	namespace math {
		//
		template<typename T>
		int signbit(T a) {
			return std::signbit(a) ? -1:1;
		}

		/// <summary>
		/// 分割单位圆
		/// </summary>
		/// <param name="num">分割数量</param>
		/// <param name="angle">被分割的圆的度数，单位角度 起始为X正半轴</param>
		/// <returns></returns>
		extern std::vector<XQ::Vec2f> lib00_utilty_API circleSplit(int num, float angle);

		extern std::vector<XQ::Vec2f> lib00_utilty_API getRectCorner(XQ::Vec2f corner1, XQ::Vec2f corner2);

		static double randon_color() {
			static std::random_device rd;
			static std::mt19937 e(rd());
			static std::uniform_real_distribution<double> u(0, 1);
			return u(e);
		}

		static unsigned int randon_ucolor() {
			static std::random_device rd;
			static std::mt19937 e(rd());
			static std::uniform_int_distribution<unsigned int> u(0, 255);
			return u(e);
		}

		template<typename T>
		static decltype(auto) randon(T min, T max, int num) {

			std::random_device rd;
			std::mt19937 e(rd());
			if constexpr (std::is_integral_v<T>) {
				std::uniform_int_distribution u(min, max);
				T* p = new T[num];
				for (int idx = 0; idx < num; ++num)
				{
					p[idx] = u(e);
				}
				return p;
			}
			if constexpr (std::is_floating_point_v<T>) {
				std::uniform_real_distribution u(min, max);
				T* p = new T[num];
				for (int idx = 0; idx < num; ++num)
				{
					p[idx] = u(e);
				}
				return p;
			}
			else {
				static_assert("true", "XQ::randon(T) 类型错误");
			}
		}

		template<typename T>
		static decltype(auto) randon(T min, T max) {
			std::random_device rd;
			std::mt19937 e(rd());
			if constexpr (std::is_integral_v<T>) {
				std::uniform_int_distribution u(min, max);
				return  u(e);	
			}
			if constexpr (std::is_floating_point_v<T>) {
				std::uniform_real_distribution u(min, max);
				return  u(e);
			}
			else {
				static_assert("true", "XQ::randon(T) 类型错误");
			}
		}
	}
}

namespace XQ {
	extern lib00_utilty_API std::wstring to_wstring_with_precision(double value, int precision);

	extern lib00_utilty_API std::string to_string_with_precision(double value, int precision);

	template< bool isReverse,size_t... Is, typename Tuple>
	auto printTupleImpl(std::ostream& os, std::index_sequence<Is...>, Tuple&& args) {
		constexpr int N = std::tuple_size_v<std::remove_reference_t<Tuple>>-1;
		if constexpr (isReverse) {
			(..., (os << std::get<N - Is>(args) << " "));
		}
		else {
			(..., (os << std::get<Is>(args) << " "));
		}
		os<<std::endl;
	}

	template<typename Tuple>
	auto printTuple(Tuple&& args,bool isReverse = false) {
		if (isReverse) {
			printTupleImpl<true>(std::cout, std::make_index_sequence<std::tuple_size_v<std::remove_reference_t<Tuple>>>(), std::forward<Tuple>(args));
		}
		else {
			printTupleImpl<false>(std::cout, std::make_index_sequence<std::tuple_size_v<std::remove_reference_t<Tuple>>>(), std::forward<Tuple>(args));
		}
	}

	template<typename ... Args>
	void print(Args&& ... args) {
		//折叠表达式，展开参数
		//一元左折叠
		//(pack op ...)  -> (((pack1 op pack2) op pack3) op ...)
		//pack =osm << std::forward<Args>(args)
		//op = ,

		/*constexpr int N = sizeof...(Args);
		((osm << std::forward<Args>(args) << " "), ...);
		osm << std::endl;*/

		auto all_args = std::forward_as_tuple(std::forward<Args>(args)... );
		//printTuple(all_args, true);
		printTuple(all_args, false);

		//osm<<std::get<0>(std::forward<decltype(all_args)>(all_args));

		//(..., (osm << std::get<N- sizeof...(Args)>(all_args) << " "));


		// 然后对剩余参数使用二元折叠
		//if constexpr (sizeof...(args) > 0) {
		//	osm <<std::get<0>(std::forward<Args>(args)...);
		//	//((osm << " " << std::forward<Args>(args)), ...);
		//}

		//一元右折叠
		// (... op pack) ->( ... op (packN-1 op packN) )
		//pack = osm << std::forward<Args>(args)
		//op = ,
		/*osm<<std::endl;
		(... , (osm<<"**" << std::forward<Args>(args)));*/
		
		//二元折叠
		//return osm;
	}
}

#define USE_PARALLEL_ALGO  1
//并行算法
namespace XQ::ParaAlgo
{
	template<class iterator>
	size_t Distance(iterator first, iterator last) {
		if constexpr (std::is_integral_v< iterator>) {
			return last -first;
		}else{
			return std::distance(first, last);	
		}
	}

	template <class _InIt, class _Diff>
	void Advance(_InIt& _Where, _Diff _Off) {
		if constexpr (std::is_integral_v< _InIt>) {
			_Where += _Off;
		}
		else {
			std::advance(_Where, _Off);
		}
	}

	/// <summary>
	/// 并行for_each
	/// </summary>
	/// <typeparam name="iterator"></typeparam>
	/// <typeparam name="Function"></typeparam>
	/// <param name="begin"></param>
	/// <param name="end"> 区间右端是开区间</param>
	/// <param name="func"></param>
	/// <param name="minBlockSize"></param>
	template <class iterator,class Function>
	void ParallelForeach(iterator begin, iterator end, Function func, const int minBlockSize = 256) {
#if USE_PARALLEL_ALGO
		unsigned int core_count = std::thread::hardware_concurrency(); //硬件支持的并发线程数量
		//auto dataSize = end - begin;
		auto dataSize = Distance(begin,end);
		auto block_size = std::max<int>(minBlockSize, std::ceil(static_cast<double>(dataSize) / core_count));// 每个核至少

		//统计需要使用多少个核
		unsigned int use_core_count = std::ceil(static_cast<double>(dataSize) / block_size);

		std::vector<std::future<void>> futures;
		for (size_t i = 0; i < use_core_count; i++) {
			auto startIdx =  i * block_size;
			auto endIdx = std::min(startIdx + block_size, dataSize);

			auto startId = begin;
			Advance(startId, startIdx);

			auto endId = begin;
			Advance(endId, endIdx);

			futures.emplace_back(
				std::async(std::launch::async, [&func,startId, endId]() {
					for (auto i = startId; i != endId; ++i)
						func(i);
					})
			);
		}

		//等待线程结束
		for (auto& f : futures) {
			f.get();
		}

#else
		for (auto i = begin; i != end; ++i)
			func(i);
#endif
	}
}
#endif