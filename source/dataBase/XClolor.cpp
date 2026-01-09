#include "XClolor.h"
namespace XQ {
	XColor::XColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
	{
		mData.setValues(r, g, b, a);
	}

	XColor::XColor(const XColor& other)
	{
		mData.setValues(other.r(), other.g(), other.b(), other.a());
	}

	XColor XColor::from_ucolor(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
	{
		return XColor(r, g, b, a);
	}

	XColor XColor::from_normalcolor(double r, double g, double b, double a)
	{
		return XColor(r * 255, g * 255, b * 255, a * 255);
	}

	int XColor::r() const
	{
		return mData.r();
	}

	int XColor::g() const
	{
		return mData.g();
	}

	int XColor::b() const
	{
		return mData.b();
	}

	int XColor::a() const
	{
		return mData.a();
	}

	double XColor::r2() const
	{
		return mData.r() / 255.;
	}

	double XColor::g2() const
	{
		return mData.g() / 255.;
	}

	double XColor::b2() const
	{
		return mData.b() / 255.;
	}

	double XColor::a2() const
	{
		return mData.a() / 255.;
	}
} // namespace XQ
