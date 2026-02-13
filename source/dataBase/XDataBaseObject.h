#pragma once

#include "databaseApi.h"
#include "XTimeStamp.h"
#include "lib00_utilty/gp/xtypelist.hpp"
#include "lib00_utilty/gp/XTraits.hpp"

#include <boost/describe.hpp>
#include <boost/mp11.hpp>
#include <boost/core/demangle.hpp>

#include <memory>
#include <map>
#include <string>
#include <functional>
#if 1
#define REGISTER_CLASS_META_DATA(class_name, ...)  \
	public:\
	BOOST_DESCRIBE_CLASS(class_name, (__VA_ARGS__), (), (), ())		\
	virtual std::vector<std::string> baseClassName() const {	\
		return XQ_META::get_parent_className<class_name>();		\
	}	\
	static std::string className() {					\
		return XQ_META::clean_type_name(boost::core::demangle(typeid(class_name).name()));\
	}	\
	virtual std::string getClassName() const {	\
		return className();	\
	}

struct XQ_META {
	static std::string clean_type_name(const std::string& name) {
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
	static std::string ClassName(T* pobj) {
		return clean_type_name(boost::core::demangle(typeid(*pobj).name()));
	}

	// ------------------------- 辅助函数：将访问修饰符转为字符串 ----------
	static std::string_view access_modifier_to_string(unsigned mod) {
		using namespace boost::describe;
		if (mod & mod_public)    return "public";
		if (mod & mod_protected) return "protected";
		if (mod & mod_private)   return "private";
		return "unknown";
	}


	template <typename T>
	static std::vector<std::string> get_parent_className() {
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
};
#endif

template<typename T>
using sptr = std::shared_ptr<T>;

template<typename T>
using csptr =const std::shared_ptr<T>;

template<typename T>
using wptr = std::weak_ptr<T>;

#if 0
template<typename Fn>
struct dataObjectFactory;

template<typename object, typename... types>
struct dataObjectFactory<object(types...)>:public object {
protected:
	dataObjectFactory(types... args) :object(args...) {}
public:
	// 静态工厂函数使用完美转发
	template<typename... Args>
	static  std::shared_ptr<object> Create(Args&&... values) {
		std::shared_ptr<object> p;
		p.reset(new dataObjectFactory<object(types...)>(std::forward<Args>(values)...));
		return p;
	}
};
#endif
template<typename object>
struct dataObjectShare:public object {
public:
	template<typename... Args, typename = std::enable_if_t<
		XTraits::is_onlyone_base_of_v<object, Args...>
		>>
	dataObjectShare(Args&&... args) :object(std::forward<Args>(args)...) {}
};


namespace __detail__{
	DEFINE_HAS_MEMBER_AND_ARGUMENT_NUM_COUNT(Init, 10);
	DEFINE_HAS_ANY_ARG_FUN(Init, 10)
}

template<typename object,typename ...Args>
std::shared_ptr<object> makeShareDbObject(Args&& ...args) {
	if constexpr(__detail__::has_Init_AnyArg_v<object> )
	{
		constexpr int argumentNum = __detail__::Init_ArgumentMaxNum<object>;
		constexpr int total_args = sizeof...(Args);

		auto all_args = std::forward_as_tuple(std::forward<Args>(args)...);
		constexpr size_t first =total_args- argumentNum;
		auto ctor_args = XTraits::extract_front_impl(std::make_index_sequence<first>{}, all_args);
		auto init_args = XTraits::extract_back_impl<first>(std::make_index_sequence<argumentNum>{}, all_args);

		std::shared_ptr<dataObjectShare<object>> shareObj =std::apply(
			[&](auto&&... args) {
				return std::make_shared<dataObjectShare<object>>(std::forward<decltype(args)>(args)...);
			}, ctor_args
		);

		std::apply([&](auto&&... args) {
			shareObj->Init(std::forward<decltype(args)>(args)...);
			}, init_args);
		return shareObj;
	}
	else {
		return std::make_shared<dataObjectShare<object>>(std::forward<Args>(args)...);
	}
	
	//该编译器不支持局部类含有模板成员函数
	/*
	struct helper :public object {
		template<typename... Types>
		helper(Types&&... arg) :object(std::forward<Types>(arg)...) {}
	};
	*/
	//return std::make_shared<helper>(std::forward<Args>(args)...);
}

/// <summary>
/// 所有类的基类，仅用来统一设计
/// </summary>
class database_API XBaseObject : public std::enable_shared_from_this<XBaseObject> {
	REGISTER_CLASS_META_DATA(XBaseObject);
protected:
	XBaseObject(){}
	virtual ~XBaseObject(){}
public:
	template<typename Derived>
	sptr<const Derived> asDerived() const {
		return std::dynamic_pointer_cast<const Derived>(this->shared_from_this());
	}

	template<typename Derived>
	sptr<Derived> asDerived() {
		return std::dynamic_pointer_cast<Derived>(this->shared_from_this());
	}
};

/// <summary>
/// 数据基类
/// </summary>
class database_API XDataBaseObject :public XBaseObject {
	REGISTER_CLASS_META_DATA(XDataBaseObject,XBaseObject);
protected:
	XDataBaseObject();
	virtual ~XDataBaseObject();
	XTimeStamp m_DataModifyTime;
public:
	void Modified() {
		m_DataModifyTime.Modified();
	}

	 XTimeStamp GetTimeStamp() const {
		return m_DataModifyTime;
	}

	//时间戳比较 this的修改时间在other的修改时间之后
	bool IsModifiedAfter(sptr<XDataBaseObject> other) const {
		return m_DataModifyTime > other->m_DataModifyTime;
	}

	bool IsModifiedBefore(sptr<XDataBaseObject> other) const {
		return m_DataModifyTime < other->m_DataModifyTime;
	}

	virtual void Init();
};

enum class database_API XDataChangeType {
	ItemDataModified,
	ItemVisibleModified,
	DataModified,
	VisibleModified,
	Update,
};
