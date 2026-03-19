#include <iostream>
#include <lib00_utilty/XUtilty.h>

#include <boost/type_index.hpp>
#include <boost/describe.hpp>
#include <boost/mp11.hpp>
#include <boost/core/demangle.hpp>

#include <string_view>
#include <xtest/XTest.h>


#define REGISTER_CLASS_META_DATA(className, ...)  \
	BOOST_DESCRIBE_CLASS(className, (__VA_ARGS__), (), (), ())		\
	virtual std::vector<std::string> BaseClassName() const {	\
		return get_parent_className<className>();		\
	}	\
	static std::string ClassName() {					\
		return clean_type_name(boost::core::demangle(typeid(className).name()));\
	}	\
	virtual std::string GetClassName() const {	\
		return ClassName();	\
	}

std::string clean_type_name(const std::string& name) {
	// 按顺序去掉 C++ 类/结构体/联合体/枚举等关键字
	constexpr std::string_view prefixes[] = {
		"class ", "struct ", "union ", "enum "
	};
	for (auto p : prefixes) {
		if (name.substr(0, p.size()) == p)
			return name.substr(p.size());
	}
	return name;   // 无匹配，直接返回原字符串
}

template<typename T>
std::string ClassName(T* pobj) {
	//return clean_type_name(boost::typeindex::type_id_runtime(*pobj).pretty_name());
	return clean_type_name(boost::core::demangle(typeid(*pobj).name()));
}


template<typename T>
std::string ClassName() {
	//return clean_type_name(boost::typeindex::type_id_runtime(*pobj).pretty_name());
	return clean_type_name(boost::core::demangle(typeid(T).name()));
}

// ------------------------- 辅助函数：将访问修饰符转为字符串 ----------
std::string_view access_modifier_to_string(unsigned mod) {
	using namespace boost::describe;
	if (mod & mod_public)    return "public";
	if (mod & mod_protected) return "protected";
	if (mod & mod_private)   return "private";
	return "unknown";
}


template <typename T>
std::vector<std::string> get_parent_className() {
	using namespace boost::describe;
	using namespace boost::mp11;

	std::vector<std::string> result;

	// 获取所有基类的描述符列表（按任意访问权限）
	using Bases = describe_bases<T, mod_any_access>;

	// 遍历列表，D 是每个基类的描述符类型
	mp_for_each<Bases>([&](auto D) {
		// D::type        - 基类类型
		// D::modifiers   - 访问权限位掩码
		// D::basename    - (C++26 提案特性，当前Boost版本可能没有) 这里用 demangle

		access_modifier_to_string(decltype(D)::modifiers);
		auto name = clean_type_name(boost::core::demangle(typeid(typename decltype(D)::type).name()));
		result.push_back(name);
		});
		return result;
}

namespace XQ {
	class foo {
		//BOOST_TYPE_INDEX_REGISTER_CLASS
	public:
		virtual ~foo() {}
		int a;
		int b;

		REGISTER_CLASS_META_DATA(foo)
	};

	class zoo {};


	class goo :public foo, public zoo {
		int c;
		int d;
		REGISTER_CLASS_META_DATA(goo,foo,zoo)
	};

	class A{};
	class B:public A{};
}

void test01() {
	XQ::print("XUtilty test");

	std::cout << boost::typeindex::type_id<XQ::foo>().pretty_name() << std::endl;

	XQ::foo* pa = new XQ::foo;
	XQ::foo* pa2 = new XQ::goo;

	std::cout << ClassName(pa) << std::endl;
	std::cout << ClassName(pa2) << std::endl;

	{
		auto name = pa->BaseClassName();
		for (auto n : name) {
			std::cout << n << std::endl;
		}
	}
	auto name = pa2->BaseClassName();
	for (auto n : name) {
		std::cout << n << std::endl;
	}
}

int main() {
	XTestApp app("XUtiltyTest");
	
	app.addCmd("test01","test01",test01);

	return app.run();
}