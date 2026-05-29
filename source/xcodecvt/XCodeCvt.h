#pragma once
#include "xcodecvtApi.h"
#include <string>

class XCODECVT_API XCodeCvt {
public:
	/**
	* @brief 将utf8编码的string转换为gbk编码的string
	*/
	static std::string utf8_to_gbk(const std::string& utf8);

	/**
	 * @brief 将gbk编码的string转换为utf-8编码的string
	 */
	static std::string gbk_to_utf8(const std::string& gbk);

	/**
	 * @brief 将utf8编码的string转换为utf-16编码的u16string
	 */
	static std::u16string utf8_to_utf16(const std::string& utf8);

	/**
	 * @brief 将utf-8编码的string转换为utf-16编码的wstring
	 */
	static std::wstring string_to_wstring(const std::string& str);

	/**
	 * @brief 将utf-16编码的wstring转换为utf-8编码的string
	 */
	static std::string wstring_to_string(const std::wstring& wstr);
};