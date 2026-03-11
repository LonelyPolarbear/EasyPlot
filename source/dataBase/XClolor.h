#pragma once
#include "dataBaseApi.h"
#include "XVector.h"
namespace XQ {
	class DATABASE_API XColor {
	public:
		XColor() = default;
		XColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255);
		XColor(const Vec4u8& c);
		XColor(const XColor& other);
		static XColor from_ucolor(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
		static XColor from_ucolor(const Vec4u8& c);
		static XColor from_normalcolor(double r, double g, double b, double a);
		static XColor from_normalcolor(const Vec4d& c);

		int r() const;
		int g() const;
		int b() const;
		int a() const;

		double r2() const;
		double g2() const;
		double b2() const;
		double a2() const;

		void setData(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255);
	protected:
		XQ::Vec4u8 mData = XQ::Vec4u8(0,0,0,255);		//ÑÕÉ«·¶Î§0-255
	public:
		friend std::ostream& operator<<(std::ostream& os, const XColor& obj) {
			os<<"("<<obj.r() << " " << obj.g() << " " << obj.b() << " " << obj.a() << ")";
			return os;
		}
	};
}