#include "myUtilty.h"
namespace myUtilty {
	double PI = 3.14159265358979323846;

	Eigen::Matrix4f Matrix::perspective(float fovy, float aspect, float znaer, float zfar)
	{
		float half_fovy = radian(0.5 * fovy);
		float tan_half_fovy = tanf(half_fovy);

		Eigen::Matrix4f m = Eigen::Matrix4f::Identity();
		m(0, 0) = (1 / tan_half_fovy) * (1 / aspect);
		m(1, 1) = 1 / tan_half_fovy;
		m(2, 2) = -(zfar + znaer) / (zfar - znaer);
		m(2, 3) = -2 * zfar * znaer / (zfar - znaer);
		m(3, 2) = -1;
		m(3, 3) = 0;
		return m;
	}

	Eigen::Matrix4f Matrix::ortho(float left, float right, float bottom, float top, float zNear, float zFar)
	{
		auto r_minus_l = right - left;
		auto r_plus_l = right + left;
		auto t_minus_b = top - bottom;
		auto t_plus_b = top + bottom;

		Eigen::Matrix4f m = Eigen::Matrix4f::Identity();
		m(0, 0) = 2 / r_minus_l;
		m(1, 1) = 2 / t_minus_b;
		m(2, 2) = -2 / (zFar - zNear);
		m(0, 3) = -r_plus_l / r_minus_l;
		m(1, 3) = -t_plus_b / t_minus_b;
		m(2, 3) = -(zFar + zNear) / (zFar - zNear);

		//mat<4, 4, T, defaultp> Result(1);
		//Result[0][0] = static_cast<T>(2) / (right - left);
		//Result[1][1] = static_cast<T>(2) / (top - bottom);
		//Result[2][2] = static_cast<T>(1) / (zFar - zNear);
		//Result[3][0] = -(right + left) / (right - left);
		//Result[3][1] = -(top + bottom) / (top - bottom);
		//Result[3][2] = -zNear / (zFar - zNear);
		return m;
	}

	float Matrix::tb_project_to_sphere(float& sphereX, float& sphereY)
	{
		float rSquared = sphereX * sphereX + sphereY * sphereY;
		float sphereZ = 0;
		if (rSquared <= 1.0) {
			// 点在球面内部：计算Z坐标
			sphereZ = sqrt(1.0 - rSquared);
		}
		else {
			// 点在球面外部：投影到球面边缘（超环面处理）
			float scale = 1.0 / sqrt(rSquared);
			sphereX *= scale;
			sphereY *= scale;
			sphereZ = 0.0;
		}
		return sphereZ;
	}

	transfomData Matrix::transformDecomposition_TSR(Eigen::Affine3f transform, std::vector<tranformType> eulerOrder) {
		Eigen::Matrix3f rotMatrix;
		Eigen::Matrix3f scaleMatrix;
		transform.computeScalingRotation(&scaleMatrix, &rotMatrix);

		std::vector<int> eulerOrder_ ;
		for (auto i : eulerOrder) {
			switch (i)
			{
			default:
				break;
			case tranformType::rotateX:
				eulerOrder_.push_back(0);
				break;
			case tranformType::rotateY:
				eulerOrder_.push_back(1);
				break;
			case tranformType::rotateZ:
				eulerOrder_.push_back(2);
				break;
			}
		}

		
		auto eulerAngles = rotMatrix.eulerAngles(eulerOrder_[0], eulerOrder_[1], eulerOrder_[2]);

		transfomData data;
		data.tx = transform.translation()(0);
		data.ty = transform.translation()(1);
		data.tz = transform.translation()(2);

		data.sx = scaleMatrix(0, 0);
		data.sy = scaleMatrix(1, 1);
		data.sz = scaleMatrix(2, 2);

		// 0->x 1->y 2->z
		//获取索引
		auto xIter = std::find(eulerOrder_.begin(), eulerOrder_.end(), 0);
		auto yIter = std::find(eulerOrder_.begin(), eulerOrder_.end(), 1);
		auto zIter = std::find(eulerOrder_.begin(), eulerOrder_.end(), 2);
		auto xIdx = std::distance(eulerOrder_.begin(), xIter);
		auto yIdx = std::distance(eulerOrder_.begin(), yIter);
		auto zIdx = std::distance(eulerOrder_.begin(), zIter);
		data.rx = Matrix::Angle(eulerAngles[xIdx]);
		data.ry = Matrix::Angle(eulerAngles[yIdx]);
		data.rz = Matrix::Angle(eulerAngles[zIdx]);

		return data;
	};

	transfomData Matrix::transformDecomposition_TRS(Eigen::Affine3f transform, std::vector<tranformType> eulerOrder) {
		Eigen::Matrix3f rotMatrix;
		Eigen::Matrix3f scaleMatrix;
		transform.computeRotationScaling(&rotMatrix, &scaleMatrix);
		std::vector<int> eulerOrder_;
		for (auto i : eulerOrder) {
			switch (i)
			{
			default:
				break;
			case tranformType::rotateX:
				eulerOrder_.push_back(0);
				break;
			case tranformType::rotateY:
				eulerOrder_.push_back(1);
				break;
			case tranformType::rotateZ:
				eulerOrder_.push_back(2);
				break;
			}
		}
		auto eulerAngles = rotMatrix.eulerAngles(eulerOrder_[0], eulerOrder_[1], eulerOrder_[2]);

		transfomData data;
		data.tx = transform.translation()(0);
		data.ty = transform.translation()(1);
		data.tz = transform.translation()(2);

		data.sx = scaleMatrix(0, 0);
		data.sy = scaleMatrix(1, 1);
		data.sz = scaleMatrix(2, 2);

		// 0->x 1->y 2->z
		//获取索引
		auto xIter = std::find(eulerOrder_.begin(), eulerOrder_.end(), 0);
		auto yIter = std::find(eulerOrder_.begin(), eulerOrder_.end(), 1);
		auto zIter = std::find(eulerOrder_.begin(), eulerOrder_.end(), 2);
		auto xIdx = std::distance(eulerOrder_.begin(), xIter);
		auto yIdx = std::distance(eulerOrder_.begin(), yIter);
		auto zIdx = std::distance(eulerOrder_.begin(), zIter);
		data.rx = Matrix::Angle(eulerAngles[xIdx]);
		data.ry = Matrix::Angle(eulerAngles[yIdx]);
		data.rz = Matrix::Angle(eulerAngles[zIdx]);

		return data;
	};

	Eigen::Matrix4f Matrix::computeMatrix(transfomData data, std::vector<tranformType> order) {
		Eigen::Affine3f transform = Eigen::Affine3f::Identity();

		std::function<void()> fn_translation = [&]() {
			transform.translation() << data.tx, data.ty, data.tz;
			};

		std::function<void()> fn_rotatex = [&]() {
			transform.rotate(Eigen::AngleAxisf(Matrix::radian(data.rx), Eigen::Vector3f::UnitX()));
			};

		std::function<void()> fn_rotatey = [&]() {
			transform.rotate(Eigen::AngleAxisf(Matrix::radian(data.ry), Eigen::Vector3f::UnitY()));
			};

		std::function<void()> fn_rotatez = [&]() {
			transform.rotate(Eigen::AngleAxisf(Matrix::radian(data.rz), Eigen::Vector3f::UnitZ()));
			};



		std::function<void()> fn_scale = [&]() {
			transform.scale(Eigen::Vector3f(data.sx, data.sy, data.sz));
			};

		std::map<tranformType, std::function<void()>> Fns{
			{tranformType::translate,fn_translation},
			{tranformType::rotateX,fn_rotatex},
			{tranformType::rotateY,fn_rotatey},
			{tranformType::rotateZ,fn_rotatez},
			{tranformType::scale,fn_scale}
		};

		for (auto i : order) {
			Fns[i]();
		}

		return transform.matrix();
	}

	Vec2i::Vec2i(const Vec2u& other) {
		x = other.x;
		y = other.y;
	}

	Vec2i::Vec2i(const Vec2f& other) {
		x = (int)other.x;
		y = (int)other.y;
	}

	Vec2i::Vec2i(const Vec2d& other) {
		x = (int)other.x;
		y = (int)other.y;	
	}
	

	Vec2f::Vec2f(const Vec2i& other) {
		x = other.x;
		y = other.y;
	}
	
	Vec2f::Vec2f(const Vec2u& other) {
		x = (float)other.x;
		y = (float)other.y;
	}

	Vec2f::Vec2f(const Vec2d& other) {
		x = other.x;
		y = other.y;
	}

    Vec2f operator*(float scalar, const Vec2f& c) {
		return Vec2f(c.x * scalar, c.y * scalar);
	}


	Vec2u::Vec2u(const Vec2i& other)
	{
		x = other.x;
		y = other.y;
	}

	Vec2u::Vec2u(const Vec2f& other)
	{
		x = (unsigned int)other.x;
		y = (unsigned int)other.y;
	}

	Vec2u::Vec2u(const Vec2d& other)
	{
		x = (unsigned int)other.x;
		y = (unsigned int)other.y;
	}

	Vec2d::Vec2d(const Vec2i& other)
	{
		x = other.x;
		y = other.y;
	}

	Vec2d::Vec2d(const Vec2u& other)
	{
		x = other.x;
		y = other.y;
	}

	Vec2d::Vec2d(const Vec2f& other)
	{
		x = other.x;
		y = other.y;
	}

}

namespace myUtilty::chrono {
	void clock::start()
	{
		start_time_point = std::chrono::high_resolution_clock::now();
	}

	void clock::restart()
	{
		start_time_point = std::chrono::high_resolution_clock::now();
	}

	int64_t clock::elapsed_ms()
	{
		auto end = std::chrono::high_resolution_clock::now();
		//std::chrono::nanoseconds 是 C++11 标准库 <chrono> 中定义的时间间隔类型，用于表示纳秒级精度的时间跨度
		auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start_time_point);
		return elapsed.count();
	}

	double clock::elapsed_seconds()
	{
		auto end = std::chrono::high_resolution_clock::now();
		//std::chrono::nanoseconds 是 C++11 标准库 <chrono> 中定义的时间间隔类型，用于表示纳秒级精度的时间跨度
		auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start_time_point);
		return (double)elapsed.count() / (double)1000.;
	}
}

namespace myUtilty::math{
	std::vector<myUtilty::Vec2f> lib00_utilty_API circleSplit(int num, float angle)
	{
		//num必须大于等于1
		if (num < 1) {
			return std::vector<myUtilty::Vec2f>();
		}

		std::vector<myUtilty::Vec2f> ret;
		float deltaAngle = angle / num;
		float angleTemp = 0;
		for (int i = 0; i <= num; i++) {
			if (i == num) {
				angleTemp = angle;
			}
			myUtilty::Vec2f v;
			v.x = std::cos(Matrix::radian(angleTemp));
			v.y = std::sin(Matrix::radian(angleTemp));
			ret.push_back(v);
			angleTemp += deltaAngle;
		}
		return ret;
	}

	std::vector<myUtilty::Vec2f>lib00_utilty_API getRectCorner(myUtilty::Vec2f corner1, myUtilty::Vec2f corner2)
	{
		auto  w =  std::abs(corner2.x - corner1.x);
		auto  h =  std::abs(corner2.y - corner1.y);

		auto x =std::min(corner1.x, corner2.x);
		auto y = std::min(corner1.y, corner2.y);

		auto p1 = myUtilty::Vec2f(x, y);
		auto p2 = myUtilty::Vec2f(x + w, y);
		auto p3 = myUtilty::Vec2f(x + w, y + h);
		auto p4 = myUtilty::Vec2f(x, y + h);

		return { p1,p2,p3,p4 };
	}
} 
