#pragma once

#include "databaseApi.h"
#include "XTimeStamp.h"
#include <memory>
#include <map>
#include <functional>
#include "lib00_utilty/gp/xtypelist.hpp"
#include "lib00_utilty/gp/XTraits.hpp"

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
