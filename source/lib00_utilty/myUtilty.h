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

namespace myUtilty {
	
	extern lib00_utilty_API std::wstring to_wstring_with_precision(double value, int precision);

	extern lib00_utilty_API std::string to_string_with_precision(double value, int precision);

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
	
	class Vec2i;
	class Vec2f;
	class Vec2u;
	class Vec2d;

	class Vec3f;
	class Vec3d;
	class Vec3u;
	class Vec3i;

	class Vec4f;
	class Vec4d;
	class Vec4u;
	class Vec4i;


	struct lib00_utilty_API Vec2i {
		int x, y;
		Vec2i(int x = 0, int y = 0) :x(x), y(y) {}
		Vec2i(const Vec2u& other);
		Vec2i(const Vec2f& other);
		Vec2i(const Vec2d& other);
	};

	struct lib00_utilty_API Vec2u {
		unsigned x, y;
		Vec2u(unsigned int x = 0, unsigned int y = 0) :x(x), y(y) {}
		Vec2u(const Vec2i& other);
		Vec2u(const Vec2f& other);
		Vec2u(const Vec2d& other);
	};

	//定义一些基本数学类型
	struct lib00_utilty_API Vec2f {
		double x, y;
		Vec2f(double x = 0, double y = 0) :x(x), y(y) {}
		Vec2f(const Vec2i& other);
		Vec2f(const Vec2u& other);
		Vec2f(const Vec2d& other);

		Vec2f operator+(const Vec2f& other) const {
			return Vec2f(x + other.x, y + other.y);
		}

		Vec2f operator-(const Vec2f& other) const {
			return Vec2f(x - other.x, y - other.y);
		}

		Vec2f operator*(const float& other) const {
			return Vec2f(x * other, y * other);
		}


		friend lib00_utilty_API Vec2f operator*(float scalar, const Vec2f& c);
	};

	struct lib00_utilty_API Vec2d {
		double x, y;
		Vec2d(double x = 0, double y = 0) :x(x), y(y) {}
		Vec2d(const Vec2i& other);
		Vec2d(const Vec2u& other);
		Vec2d(const Vec2f& other);
	};

	struct lib00_utilty_API Vec3f {
		float x, y,z;
		Vec3f(float x = 0, float y = 0, float z = 0) :x(x), y(y), z(z) {}
	};

	struct lib00_utilty_API Vec3d {
		double x, y, z;
		Vec3d(double x = 0, double y = 0, double z = 0) :x(x), y(y), z(z) {}
	};

	struct lib00_utilty_API Vec3u {
		unsigned int x, y, z;
		Vec3u(unsigned int x = 0, unsigned int y = 0, unsigned int z = 0) :x(x), y(y), z(z) {}
	};

	struct lib00_utilty_API Vec3i {
		 int x, y, z;
		 Vec3i(int x = 0, int y = 0, int z = 0) :x(x), y(y), z(z) {}
	};

	//
	struct lib00_utilty_API Vec4f {
		float x, y, z,w;
		Vec4f(float x = 0, float y = 0, float z = 0,float w=0) :x(x), y(y), z(z),w(w) {}
	};

	struct lib00_utilty_API Vec4d {
		double x, y, z,w;
		Vec4d(double x = 0, double y = 0, double z = 0, double w = 0) :x(x), y(y), z(z), w(w) {}
		
	};

	struct lib00_utilty_API Vec4u {
		unsigned int x, y, z,w;
		Vec4u(unsigned int x = 0, unsigned int y = 0, unsigned int z = 0, unsigned int w = 0) :x(x), y(y), z(z), w(w) {}
	};

	

	struct lib00_utilty_API Vec4i {
		int x, y, z,w;
		Vec4i(int x = 0, int y = 0, int z = 0, int w = 0) :x(x), y(y), z(z), w(w) {}
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
		extern std::vector<myUtilty::Vec2f> lib00_utilty_API circleSplit(int num, float angle);

		extern std::vector<myUtilty::Vec2f> lib00_utilty_API getRectCorner(myUtilty::Vec2f corner1, myUtilty::Vec2f corner2);

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
				static_assert("true", "myUtilty::randon(T) 类型错误");
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
				static_assert("true", "myUtilty::randon(T) 类型错误");
			}
		}
	}
}

#define USE_PARALLEL_ALGO  1
//并行算法
namespace myUtilty::ParaAlgo
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