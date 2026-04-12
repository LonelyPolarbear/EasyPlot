#pragma once

#include "xtypelist.hpp"
#include <type_traits>
#include<memory>
#include <tuple>

// 检测特定返回类型的重载
/*
template<typename T,typename Fn>
struct has_serialize_returning;

template<typename T, typename Ret, typename... Args>
struct has_serialize_returning<T,Ret(Args...)> {
private:
	template<typename U>
	static auto test(int) -> std::is_same<
		decltype(std::declval<U>().serialize(std::declval<Args>()...)),Ret
	>;

	template<typename U>
	static std::false_type test(...);

public:
	static constexpr bool value = decltype(test<T>(0))::value;
};

template<typename T, typename Fn>
constexpr bool has_serialize_returning_v = has_serialize_returning<T,Fn>::value;
*/
namespace XTraits {
	struct anything {
		template<typename T>
		operator T() const { return T{}; }
	};

	// 自定义 constexpr max 函数
	constexpr int max_value(int a, int b) {
		return a > b ? a : b;
	}

	constexpr int min_value(int a, int b) {
		return a < b ? a : b;
	}

	template<typename... Args>
	constexpr int max_value(int a, int b, Args... args) {
		return max_value(a, max_value(b, args...));
	}
}

#define DEFINE_ARGS(n) DEFINE_ARGS_##n
#define DEFINE_ARGS_0 
#define DEFINE_ARGS_1 std::declval<XTraits::anything>()
#define DEFINE_ARGS_2 DEFINE_ARGS_1, std::declval<XTraits::anything>()
#define DEFINE_ARGS_3 DEFINE_ARGS_2, std::declval<XTraits::anything>()
#define DEFINE_ARGS_4 DEFINE_ARGS_3, std::declval<XTraits::anything>()
#define DEFINE_ARGS_5 DEFINE_ARGS_4, std::declval<XTraits::anything>()
#define DEFINE_ARGS_6 DEFINE_ARGS_5, std::declval<XTraits::anything>()
#define DEFINE_ARGS_7 DEFINE_ARGS_6, std::declval<XTraits::anything>()
#define DEFINE_ARGS_8 DEFINE_ARGS_7, std::declval<XTraits::anything>()
#define DEFINE_ARGS_9 DEFINE_ARGS_8, std::declval<XTraits::anything>()
#define DEFINE_ARGS_10 DEFINE_ARGS_9, std::declval<XTraits::anything>()

//////////////////////////////////////////////////////////////////////////
//判断某个成员函数是否具有某个参数列表的函数
#define HAS_MEMBER_FUNCTION(funName) \
template<typename className,typename Fn> \
struct has_##funName; \
template<typename T, typename Ret, typename... Args> \
struct has_##funName<T,Ret(Args...)> { \
private: \
	template<typename U> \
	static auto test(int) -> std::is_same< \
		decltype(std::declval<U>().funName(std::declval<Args>()...)),Ret \
	>; \
	template<typename U> \
	static std::false_type test(...); \
public: \
	static constexpr bool value = decltype(test<T>(0))::value; \
};\
template<typename T, typename Fn>\
constexpr bool has_##funName##_v = has_##funName<T,Fn>::value;

HAS_MEMBER_FUNCTION(getClassName)

//////////////////////////////////////////////////////////////////////////
//判断某个成员函数是否具有某个函数，并不关心参数列表
#if 1
#define DEFINE_HAS_TEST_N_ARGS(funName, n) \
template<typename T, typename = void> \
struct has_##funName##_##n##_args : std::false_type {}; \
template<typename T> \
struct has_##funName##_##n##_args<T, std::void_t< \
decltype(std::declval<T>().funName(DEFINE_ARGS(n))) \
>> : std::true_type {};\
template<typename T>\
constexpr bool has_##funName##_##n##_args_v = has_##funName##_##n##_args<T>::value; \
template<typename T>	\
constexpr int funName##_parameterNum_##n = has_##funName##_##n##_args_v<T> ? n:0;
#else
#define DEFINE_HAS_TEST_N_ARGS(funName, n) \
template<typename T> struct has_##funName##_##n##_args :T { \
	template<typename U>			\
	static  auto test(int) -> decltype(		\
		std::declval<U>().funName(DEFINE_ARGS(n)),  \
		std::true_type{}   \
		); \
	template<typename U>  \
	static  std::false_type test(...);  \
	static constexpr bool value = decltype(test<has_##funName##_##n##_args>(0))::value; \
};\
template<typename T>\
constexpr bool has_##funName##_##n##_args_v = has_##funName##_##n##_args<T>::value; \
template<typename T>	\
constexpr int funName##_parameterNum_##n = has_##funName##_##n##_args_v<T> ? n:0;
#endif

	//decltype(std::declval<T>().funName(DEFINE_ARGS(n))) \
//////////////////////////////////////////////////////////////////////////
//判断某个成员函数是否具有某个参数列表的函数
#define DEFINE_HAS_MEMBER_N_ARGS(funName,n) DEFINE_HAS_MEMBER_N_ARGS_##n(funName)
#define DEFINE_HAS_MEMBER_N_ARGS_0(funName) DEFINE_HAS_TEST_N_ARGS(funName,0)
#define DEFINE_HAS_MEMBER_N_ARGS_1(funName) DEFINE_HAS_TEST_N_ARGS(funName,1) DEFINE_HAS_MEMBER_N_ARGS_0(funName)
#define DEFINE_HAS_MEMBER_N_ARGS_2(funName) DEFINE_HAS_TEST_N_ARGS(funName,2) DEFINE_HAS_MEMBER_N_ARGS_1(funName)
#define DEFINE_HAS_MEMBER_N_ARGS_3(funName) DEFINE_HAS_TEST_N_ARGS(funName,3) DEFINE_HAS_MEMBER_N_ARGS_2(funName)
#define DEFINE_HAS_MEMBER_N_ARGS_4(funName) DEFINE_HAS_TEST_N_ARGS(funName,4) DEFINE_HAS_MEMBER_N_ARGS_3(funName)
#define DEFINE_HAS_MEMBER_N_ARGS_5(funName) DEFINE_HAS_TEST_N_ARGS(funName,5) DEFINE_HAS_MEMBER_N_ARGS_4(funName)
#define DEFINE_HAS_MEMBER_N_ARGS_6(funName) DEFINE_HAS_TEST_N_ARGS(funName,6) DEFINE_HAS_MEMBER_N_ARGS_5(funName)
#define DEFINE_HAS_MEMBER_N_ARGS_7(funName) DEFINE_HAS_TEST_N_ARGS(funName,7) DEFINE_HAS_MEMBER_N_ARGS_6(funName)
#define DEFINE_HAS_MEMBER_N_ARGS_8(funName) DEFINE_HAS_TEST_N_ARGS(funName,8) DEFINE_HAS_MEMBER_N_ARGS_7(funName)
#define DEFINE_HAS_MEMBER_N_ARGS_9(funName) DEFINE_HAS_TEST_N_ARGS(funName,9) DEFINE_HAS_MEMBER_N_ARGS_8(funName)
#define DEFINE_HAS_MEMBER_N_ARGS_10(funName) DEFINE_HAS_TEST_N_ARGS(funName,10) DEFINE_HAS_MEMBER_N_ARGS_9(funName)

#define DEFINE_FUN_ANY_ARGS(n,funName,ClassName)  DEFINE_FUN_ANY_ARGS_##n(funName,ClassName)
#define DEFINE_FUN_ANY_ARGS_0(funName,ClassName) has_##funName##_0_args_v<ClassName>
#define DEFINE_FUN_ANY_ARGS_1(funName,ClassName) DEFINE_FUN_ANY_ARGS_0(funName,ClassName) || has_##funName##_1_args_v<ClassName>
#define DEFINE_FUN_ANY_ARGS_2(funName,ClassName) DEFINE_FUN_ANY_ARGS_1(funName,ClassName) ||   has_##funName##_2_args_v<ClassName>
#define DEFINE_FUN_ANY_ARGS_3(funName,ClassName) DEFINE_FUN_ANY_ARGS_2(funName,ClassName) ||   has_##funName##_3_args_v<ClassName>
#define DEFINE_FUN_ANY_ARGS_4(funName,ClassName) DEFINE_FUN_ANY_ARGS_3(funName,ClassName) ||   has_##funName##_4_args_v<ClassName>
#define DEFINE_FUN_ANY_ARGS_5(funName,ClassName) DEFINE_FUN_ANY_ARGS_4(funName,ClassName) ||   has_##funName##_5_args_v<ClassName>
#define DEFINE_FUN_ANY_ARGS_6(funName,ClassName) DEFINE_FUN_ANY_ARGS_5(funName,ClassName) ||  has_##funName##_6_args_v<ClassName>
#define DEFINE_FUN_ANY_ARGS_7(funName,ClassName) DEFINE_FUN_ANY_ARGS_6(funName,ClassName) ||  has_##funName##_7_args_v<ClassName>
#define DEFINE_FUN_ANY_ARGS_8(funName,ClassName) DEFINE_FUN_ANY_ARGS_7(funName,ClassName) || has_##funName##_8_args_v<ClassName>
#define DEFINE_FUN_ANY_ARGS_9(funName,ClassName) DEFINE_FUN_ANY_ARGS_8(funName,ClassName) || has_##funName##_9_args_v<ClassName>
#define DEFINE_FUN_ANY_ARGS_10(funName,ClassName) DEFINE_FUN_ANY_ARGS_9(funName,ClassName) ||  has_##funName##_10_args_v<ClassName>

#define DEFINE_FUN_ARGUMENT_NUM_ARGS(n,funName,ClassName) DEFINE_FUN_ARGUMENT_NUM_ARGS_##n(funName,ClassName)
#define DEFINE_FUN_ARGUMENT_NUM_ARGS_0(funName,ClassName) funName##_parameterNum_0<ClassName>
#define DEFINE_FUN_ARGUMENT_NUM_ARGS_1(funName,ClassName) DEFINE_FUN_ARGUMENT_NUM_ARGS_0(funName,ClassName) +funName##_parameterNum_1<ClassName>
#define DEFINE_FUN_ARGUMENT_NUM_ARGS_2(funName,ClassName) DEFINE_FUN_ARGUMENT_NUM_ARGS_1(funName,ClassName) +funName##_parameterNum_2<ClassName>
#define DEFINE_FUN_ARGUMENT_NUM_ARGS_3(funName,ClassName) DEFINE_FUN_ARGUMENT_NUM_ARGS_2(funName,ClassName) +funName##_parameterNum_3<ClassName>
#define DEFINE_FUN_ARGUMENT_NUM_ARGS_4(funName,ClassName) DEFINE_FUN_ARGUMENT_NUM_ARGS_3(funName,ClassName) +funName##_parameterNum_4<ClassName>
#define DEFINE_FUN_ARGUMENT_NUM_ARGS_5(funName,ClassName) DEFINE_FUN_ARGUMENT_NUM_ARGS_4(funName,ClassName) +funName##_parameterNum_5<ClassName>
#define DEFINE_FUN_ARGUMENT_NUM_ARGS_6(funName,ClassName) DEFINE_FUN_ARGUMENT_NUM_ARGS_5(funName,ClassName) +funName##_parameterNum_6<ClassName>
#define DEFINE_FUN_ARGUMENT_NUM_ARGS_7(funName,ClassName) DEFINE_FUN_ARGUMENT_NUM_ARGS_6(funName,ClassName) +funName##_parameterNum_7<ClassName>
#define DEFINE_FUN_ARGUMENT_NUM_ARGS_8(funName,ClassName) DEFINE_FUN_ARGUMENT_NUM_ARGS_7(funName,ClassName) +funName##_parameterNum_8<ClassName>
#define DEFINE_FUN_ARGUMENT_NUM_ARGS_9(funName,ClassName) DEFINE_FUN_ARGUMENT_NUM_ARGS_8(funName,ClassName) +funName##_parameterNum_9<ClassName>
#define DEFINE_FUN_ARGUMENT_NUM_ARGS_10(funName,ClassName) DEFINE_FUN_ARGUMENT_NUM_ARGS_9(funName,ClassName) +funName##_parameterNum_10<ClassName>

//#define DEFINE_FUN_MAX_NUM_ARGS(n,funName,ClassName) DEFINE_FUN_MAX_NUM_ARGS_##n(funName,ClassName)
//#define DEFINE_FUN_MAX_NUM_ARGS_0(funName,ClassName) std::max( funName##_parameterNum_0<ClassName>,0)
//#define DEFINE_FUN_MAX_NUM_ARGS_1(funName,ClassName) std::max(DEFINE_FUN_MAX_NUM_ARGS_0(funName,ClassName),funName##_parameterNum_1<ClassName>)
//#define DEFINE_FUN_MAX_NUM_ARGS_2(funName,ClassName) std::max(DEFINE_FUN_MAX_NUM_ARGS_1(funName,ClassName) ,funName##_parameterNum_2<ClassName>)
//#define DEFINE_FUN_MAX_NUM_ARGS_3(funName,ClassName) std::max(DEFINE_FUN_MAX_NUM_ARGS_2(funName,ClassName) ,funName##_parameterNum_3<ClassName>)
//#define DEFINE_FUN_MAX_NUM_ARGS_4(funName,ClassName) std::max(DEFINE_FUN_MAX_NUM_ARGS_3(funName,ClassName) ,funName##_parameterNum_4<ClassName>)
//#define DEFINE_FUN_MAX_NUM_ARGS_5(funName,ClassName) std::max(DEFINE_FUN_MAX_NUM_ARGS_4(funName,ClassName) ,funName##_parameterNum_5<ClassName>)
//#define DEFINE_FUN_MAX_NUM_ARGS_6(funName,ClassName) std::max(DEFINE_FUN_MAX_NUM_ARGS_5(funName,ClassName) ,funName##_parameterNum_6<ClassName>)
//#define DEFINE_FUN_MAX_NUM_ARGS_7(funName,ClassName) std::max(DEFINE_FUN_MAX_NUM_ARGS_6(funName,ClassName) ,funName##_parameterNum_7<ClassName>)
//#define DEFINE_FUN_MAX_NUM_ARGS_8(funName,ClassName) std::max(DEFINE_FUN_MAX_NUM_ARGS_7(funName,ClassName) ,funName##_parameterNum_8<ClassName>)
//#define DEFINE_FUN_MAX_NUM_ARGS_9(funName,ClassName) std::max(DEFINE_FUN_MAX_NUM_ARGS_8(funName,ClassName) ,funName##_parameterNum_9<ClassName>)
//#define DEFINE_FUN_MAX_NUM_ARGS_10(funName,ClassName) std::max(DEFINE_FUN_MAX_NUM_ARGS_9(funName,ClassName) ,funName##_parameterNum_10<ClassName>)

#define DEFINE_FUN_MAX_NUM_ARGS(n,funName,ClassName) XTraits::max_value(DEFINE_FUN_MAX_NUM_ARGS_##n(funName,ClassName))
#define DEFINE_FUN_MAX_NUM_ARGS_0(funName,ClassName) funName##_parameterNum_0<ClassName>
#define DEFINE_FUN_MAX_NUM_ARGS_1(funName,ClassName) DEFINE_FUN_MAX_NUM_ARGS_0(funName,ClassName),funName##_parameterNum_1<ClassName>
#define DEFINE_FUN_MAX_NUM_ARGS_2(funName,ClassName) DEFINE_FUN_MAX_NUM_ARGS_1(funName,ClassName) ,funName##_parameterNum_2<ClassName>
#define DEFINE_FUN_MAX_NUM_ARGS_3(funName,ClassName) DEFINE_FUN_MAX_NUM_ARGS_2(funName,ClassName) ,funName##_parameterNum_3<ClassName>
#define DEFINE_FUN_MAX_NUM_ARGS_4(funName,ClassName) DEFINE_FUN_MAX_NUM_ARGS_3(funName,ClassName) ,funName##_parameterNum_4<ClassName>
#define DEFINE_FUN_MAX_NUM_ARGS_5(funName,ClassName) DEFINE_FUN_MAX_NUM_ARGS_4(funName,ClassName) ,funName##_parameterNum_5<ClassName>
#define DEFINE_FUN_MAX_NUM_ARGS_6(funName,ClassName) DEFINE_FUN_MAX_NUM_ARGS_5(funName,ClassName) ,funName##_parameterNum_6<ClassName>
#define DEFINE_FUN_MAX_NUM_ARGS_7(funName,ClassName) DEFINE_FUN_MAX_NUM_ARGS_6(funName,ClassName) ,funName##_parameterNum_7<ClassName>
#define DEFINE_FUN_MAX_NUM_ARGS_8(funName,ClassName) DEFINE_FUN_MAX_NUM_ARGS_7(funName,ClassName) ,funName##_parameterNum_8<ClassName>
#define DEFINE_FUN_MAX_NUM_ARGS_9(funName,ClassName) DEFINE_FUN_MAX_NUM_ARGS_8(funName,ClassName) ,funName##_parameterNum_9<ClassName>
#define DEFINE_FUN_MAX_NUM_ARGS_10(funName,ClassName) DEFINE_FUN_MAX_NUM_ARGS_9(funName,ClassName) ,funName##_parameterNum_10<ClassName>

//////////////////////////////////////////////////////////////////////////
//获取某个成员函数的重载版本的参数总和数量
#define DEFINE_FUN_ARGUMENT_ACCUMALE_NUM(funName,n)		\
template<typename T>\
constexpr int funName##_ArgumentAccuNum = DEFINE_FUN_ARGUMENT_NUM_ARGS(n, Init, T);

#define DEFINE_HAS_ANY_ARG_FUN(funName,n)		\
template<typename T>\
constexpr bool has_##funName##_AnyArg_v = DEFINE_FUN_ANY_ARGS(n, Init, T);

//获取某个成员函数的重载版本的最大数量
#define DEFINE_FUN_ARGUMENT_MAX_NUM(funName,n)		\
template<typename T>\
constexpr int funName##_ArgumentMaxNum = DEFINE_FUN_MAX_NUM_ARGS(n, Init, T);

//获取某个成员函数的重载版本的最小数量
#define DEFINE_FUN_ARGUMENT_MIN_NUM(funName,n)		\
template<typename T>\
constexpr int funName##_ArgumentMinNum = DEFINE_FUN_MIN_NUM_ARGS(n, Init, T);

#define DEFINE_HAS_MEMBER_AND_ARGUMENT_NUM_COUNT(funName,n)		\
DEFINE_HAS_MEMBER_N_ARGS(funName, n)		\
DEFINE_FUN_ARGUMENT_ACCUMALE_NUM(funName, n)\
DEFINE_FUN_ARGUMENT_MAX_NUM(funName, n)


namespace XTraits {
	template<typename T>
	struct traits_class {
		using classType = T;
	};

	template<typename T>
	struct traits_class<std::shared_ptr<T>> {
		using classType = T;
	};

	template<typename T>
	struct traits_class<std::unique_ptr<T>> {
		using classType = T;
	};

	template<typename T>
	struct traits_class<std::weak_ptr<T>> {
		using classType = T;
	};

	template<typename T>
	using traits_class_t = typename traits_class<T>::classType;

	template<typename ... Args>
	struct conjunction {
		static constexpr bool value = TL::TL_is<TL::typelsit<Args...>,std::is_arithmetic>::value;
	};

	template<typename ...Args>
	constexpr bool conjunction_v =  conjunction<Args...>::value;


	template<typename object,typename ...Args>
	constexpr bool is_onlyone_base_of_v =
	!(sizeof...(Args) == 1 && std::is_base_of_v
		<object,
		TL::TL_Front_t<
		TL::TL_pushback_t< TL::typelsit< std::decay_t<Args>...>, void>
		>
		>);

	template<size_t... Is, typename Tuple>
	auto extract_front_impl(std::index_sequence<Is...>, Tuple&& all_args) {
		return std::make_tuple(std::get<Is>(std::forward<Tuple>(all_args))...);
	}

	template<size_t Offset, size_t... Is, typename Tuple>
	auto extract_back_impl(std::index_sequence<Is...>, Tuple&& all_args) {
		return std::make_tuple(std::get<Offset + Is>(std::forward<Tuple>(all_args))...);
	}

	template<typename T, typename = void>
	struct has_equal_operator : std::false_type {};

	// 特化：当表达式 t == u 有效时
	template<typename T>
	struct has_equal_operator<T,
		std::void_t<decltype(std::declval<const T&>() == std::declval<const T&>())>>
		: std::true_type {};

	// 辅助变量模板（C++17）
	template<typename T>
	constexpr bool has_equal_operator_v = has_equal_operator<T>::value;
}