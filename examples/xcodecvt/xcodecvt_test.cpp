

#include <xtest/XTest.h>
#include <xlog/XLogger.h>
#include <xcodecvt/XCodeCvt.h>
#include <iostream>
#include <iomanip>
/*
UTF-8（变长编码）：使用 1 到 4 个字节来表示一个字符。ASCII 字符仅占 1 个字节；大部分常用汉字占 3 个字节；极少数生僻字或表情符号占 4 个字节
UTF-16（混合长度）：以 16 位（2 字节）为基本单元,绝大多数常用字符（如英文、中文），固定使用 2 个字节,部分占用4字节
UTF-32（全称 32-bit Unicode Transformation Format）是 Unicode 字符集的一种具体编码实现方式。
			它的核心特征是定长编码，即对每一个合法的 Unicode 码点（从 U+0000 到 U+10FFFF），都严格使用固定的 4 字节（32 位） 来存储。

*/

/*
std::u16string 固定是utf-16编码
std::wstring因为平台的差异，可能是utf-16，也可能是utf-32
*/

/*
宋 UTF-8 E5AE8B		GB2312 CBCE		UTF-16BE 5B8B		UTF-16BE 8B5B		Unicode 00005B8B
伟 UTF-8 E4BC9F		GB2312 CEB0		UTF-16BE 4F1F		UTF-16BE 1F4F		Unicode 00004F1F
军 UTF-8 E5869B		GB2312 BEFC		UTF-16BE 519B		UTF-16BE 9B51		Unicode 0000519B	
*/
void test01() {
	std::string str("宋伟军");
	std::cout<<str<<std::endl;
	auto p = str.c_str();
	for (int i = 0; i < str.size(); i++) {
		std::cout<<std::hex<< std::setw(2) <<(unsigned int)p[i]<<std::endl;
	}
	std::cout<<"------------------\n";
	std::string gbk=XCodeCvt::utf8_to_gbk(str);
	std::cout << gbk << std::endl;
	p = gbk.c_str();
	for (int i = 0; i < gbk.size(); i++) {
		std::cout << std::setfill('0') <<std::hex << std::setw(2) << (unsigned int)p[i] << std::endl;
	}
}

void test02() {
	std::wstring str_w(L"宋伟军");
	auto p = str_w.c_str();
	for (int i = 0; i < str_w.size(); i++) {
		std::cout << std::setfill('0') <<std::hex << std::setw(2) << (unsigned int)p[i] << std::endl;		//5b8b 4f1f 519b
	}

	std::u16string str_u16 = u"宋伟军";
	std::string str ="宋伟军";

	auto wstr2str = XCodeCvt::wstring_to_string(str_w);


	std::u16string emoji = u"?";
	int i=0;
}

int main() {
	XTestApp app;
	app.setName("XlogTest")->setDesc("字符编码转换");

	app.addCmd("test01", "string", test01);
	app.addCmd("test02", "wstring", test02);
	
	return app.run();
}
