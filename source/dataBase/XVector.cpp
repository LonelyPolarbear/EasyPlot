#include "XVector.h"
namespace XQ {
	template class DATABASE_API Vector<2, char>;
	template class DATABASE_API Vector<2, unsigned char>;
	template class DATABASE_API Vector<2, float>;
	template class DATABASE_API Vector<2, double>;
	template class DATABASE_API Vector<2, int>;
	template class DATABASE_API Vector<2, unsigned int>;

	template class DATABASE_API Vector<3, char>;
	template class DATABASE_API Vector<3, unsigned char>;
	template class DATABASE_API Vector<3, float>;
	template class DATABASE_API Vector<3, double>;
	template class DATABASE_API Vector<3, int>;
	template class DATABASE_API Vector<3, unsigned int>;

	template class DATABASE_API Vector<4, char>;
	template class DATABASE_API Vector<4, unsigned char>;
	template class DATABASE_API Vector<4, float>;
	template class DATABASE_API Vector<4, double>;
	template class DATABASE_API Vector<4, int>;
	template class DATABASE_API Vector<4, unsigned int>;

	static float radian(float v) {
		double PI = 3.14159265358979323846;
		return v * PI / 180.0;
	}

	namespace math {
		std::vector<XQ::Vec2f>DATABASE_API circleSplit(int num, float angle)
		{
			//num必须大于等于1
			if (num < 1) {
				return std::vector<XQ::Vec2f>();
			}

			std::vector<XQ::Vec2f> ret;
			float deltaAngle = angle / num;
			float angleTemp = 0;
			for (int i = 0; i <= num; i++) {
				if (i == num) {
					angleTemp = angle;
				}
				XQ::Vec2f v;
				v.x() = std::cos(radian(angleTemp));
				v.y() = std::sin(radian(angleTemp));
				ret.push_back(v);
				angleTemp += deltaAngle;
			}
			return ret;
		}

		std::vector<XQ::Vec2f>DATABASE_API getRectCorner(XQ::Vec2f corner1, XQ::Vec2f corner2)
		{
			auto  w = std::abs(corner2.x() - corner1.x());
			auto  h = std::abs(corner2.y() - corner1.y());

			auto x = std::min(corner1.x(), corner2.x());
			auto y = std::min(corner1.y(), corner2.y());

			auto p1 = XQ::Vec2f(x, y);
			auto p2 = XQ::Vec2f(x + w, y);
			auto p3 = XQ::Vec2f(x + w, y + h);
			auto p4 = XQ::Vec2f(x, y + h);

			return { p1,p2,p3,p4 };
		}
	}

}