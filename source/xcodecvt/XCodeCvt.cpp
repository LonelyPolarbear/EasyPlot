#include "xcodecvt/XCodeCvt.h"
#include <boost/locale.hpp>

std::string XCodeCvt::utf8_to_gbk(const std::string& utf8)
{
	return boost::locale::conv::from_utf(utf8, "GBK");
}

std::string XCodeCvt::gbk_to_utf8(const std::string& gbk)
{
	return boost::locale::conv::to_utf<char>(gbk, "GBK");
}

std::u16string XCodeCvt::utf8_to_utf16(const std::string& utf8)
{
	return boost::locale::conv::utf_to_utf<char16_t>(utf8);
}

std::wstring XCodeCvt::string_to_wstring(const std::string& str)
{
	return boost::locale::conv::utf_to_utf<wchar_t>(str);
}

std::string XCodeCvt::wstring_to_string(const std::wstring& wstr)
{
	return boost::locale::conv::utf_to_utf<char>(wstr);
}
