#pragma once
#include "databaseApi.h"
#include "XVector.h"
namespace XQ {
	class database_API XColor {
	public:
		XColor() = default;
		XColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255);
		XColor(const XColor& other);
		static XColor from_ucolor(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
		static XColor from_normalcolor(double r, double g, double b, double a);

		int r() const;
		int g() const;
		int b() const;
		int a() const;

		double r2() const;
		double g2() const;
		double b2() const;
		double a2() const;
	protected:
		XQ::Vec4u8 mData = XQ::Vec4u8(0,0,0,255);		//ÑÕÉ«·¶Î§0-255
	};
}