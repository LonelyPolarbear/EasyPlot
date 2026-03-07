#include "XDataPath.h"
#include<regex>

static std::vector<std::string> split(const std::string& input, const std::string& regex_str) {
	std::regex re(regex_str);
	std::sregex_token_iterator it(input.begin(), input.end(), re, -1);
	std::sregex_token_iterator reg_end;
	return { it, reg_end };
}

XQ::XDataPath XQ::XDataPath::fromStr(const std::string& str)
{
    // /分割的若干个字符串
	auto n = split(str,"/");
	XDataPath path;
	path.names = n;
	return path;
}

XQ::XDataPath::XDataPath()
{

}
