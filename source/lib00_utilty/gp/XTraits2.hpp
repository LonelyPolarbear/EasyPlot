#pragma once

#include "xtypelist.hpp"
#include <type_traits>
#include<memory>
#include <tuple>

namespace XTraits {
	template <typename Fn_t>
	struct OverLoad;

	template <typename retType, typename... parmTypes>
	struct OverLoad<retType(parmTypes...)>
	{
		template <typename T>
		static constexpr decltype(auto) of(retType(T::* f)(parmTypes...))
		{
			return f;
		}

		template <typename T>
		static constexpr decltype(auto) const_of(retType(T::* f)(parmTypes...) const)
		{
			return f;
		}

		static constexpr decltype(auto) cStyleOf(retType(*f)(parmTypes...))
		{
			return f;
		}
	};

	//const成员函数指针
	template <typename T>
	struct is_const_member_function_pointer : std::false_type {};

	template <typename R, typename C, typename... Args>
	struct is_const_member_function_pointer<R(C::*)(Args...)> : std::false_type {};

	template <typename R, typename C, typename... Args>
	struct is_const_member_function_pointer<R(C::*)(Args...) const> : std::true_type {};

	//非const成员函数指针
	template <typename T>
	struct is_member_function_pointer : std::false_type {};

	template <typename R, typename C, typename... Args>
	struct is_member_function_pointer<R(C::*)(Args...)> : std::true_type {};


	//普通函数指针
	template <typename T>
	struct is_cfunction_pointer : std::false_type {};

	template <typename R,typename... Args>
	struct is_cfunction_pointer<R(*)(Args...)> : std::true_type {};

	template<typename T,typename R>
	constexpr decltype(auto) OverLoadOf(R f) {
		if constexpr (is_cfunction_pointer<R>::value) {
			return OverLoad<T>::cStyleOf(f);
		}
		else if constexpr (is_member_function_pointer<R>::value) {
			return OverLoad<T>::of(f);
		}
		else if constexpr (is_const_member_function_pointer<R>::value) {
			return OverLoad<T>::const_of(f);
		}
	}
}