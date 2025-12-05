/**
* @file         singlefactory.hpp
* @brief       泛型工厂模板类，实现泛型工厂模式
* @author     宋伟军
* @date        2025.6.27
* @license Copyright(c)2023, 英特工程仿真技术(大连)有限公司版权所有
*/

#ifndef SFACTORY_HPP
#define SFACTORY_HPP
#include <map>
#include <list>
#include<memory>
#include <mutex>
#include <functional>

namespace TL_singleFactory {
	
	struct empty_type {};
	template<size_t... Is, typename Tuple>
	auto extract_front_impl(std::index_sequence<Is...>, Tuple&& all_args) {
		return std::make_tuple(std::get<Is>(std::forward<Tuple>(all_args))...);
	}

	template<size_t Offset, size_t... Is, typename Tuple>
	auto extract_back_impl(std::index_sequence<Is...>, Tuple&& all_args) {
		return std::make_tuple(std::get<Offset + Is>(std::forward<Tuple>(all_args))...);
	}

	template<typename base, typename object, typename... types>
	struct singleFactory_createSelf :public object {
		singleFactory_createSelf(types... args) :object(args...) {}
		static base* crateSelf(types... values) {
			return new singleFactory_createSelf<base, object, types...>(values...);
		}
	};

	template<typename F>
	struct getReturnType;

	template<typename R, typename ... types>
	struct getReturnType<R(types...)> {
		using type = R;
	};
}

template<typename identifier, typename ctorType, typename initType, typename = std::enable_if_t< std::has_virtual_destructor_v<typename TL_singleFactory::getReturnType<ctorType>::type>>
>
class SFactory;     //主模版

template<typename product, typename identifier, typename... ctorTypes, typename... initTypes>
class SFactory<identifier, product(ctorTypes...), void(initTypes...)> {
	using createFun = product * (*)(ctorTypes...);
	using initFn = std::function< void(product*, initTypes...)>;
public:
	static SFactory& Instance() {
		static SFactory instance;
		return instance;
	}

private:
	template<typename Container, typename... Args>
	std::shared_ptr<product> makeShareImpl(identifier id, Container& container, Args&&... args) {
		constexpr std::size_t N = sizeof...(ctorTypes);
		constexpr std::size_t total_args = sizeof...(Args);

		if constexpr (std::is_same_v<Container, TL_singleFactory::empty_type>) {
			// 普通创建，不检查缓存
		}
		else {
			// 检查缓存
			auto it = container.find(id);
			if (it != container.end()) {
				if constexpr (std::is_same_v<Container,
					std::map<identifier, std::weak_ptr<product>>>) {
					auto sp = it->second.lock();
					if (sp) return sp;
					container.erase(id);
				}
				else {
					return it->second;
				}
			}
		}

		auto all_args = std::forward_as_tuple(std::forward<Args>(args)...);
		constexpr size_t rest = total_args - N;
		auto ctor_args = TL_singleFactory::extract_front_impl(std::make_index_sequence<N>{}, all_args);
		auto init_args = TL_singleFactory::extract_back_impl<N>(std::make_index_sequence<rest>{}, all_args);

		product* p = std::apply(s_NewMethod[id], ctor_args);
		auto result = std::shared_ptr<product>(p);

		if (!std::apply([this, id, p](auto&&... init_args) {
			return this->init(id, p, std::forward<decltype(init_args)>(init_args)...);
			}, init_args)) {
			return nullptr;
		}

		if constexpr (!std::is_same_v<Container, TL_singleFactory::empty_type>) {
			container[id] = result;
		}

		return result;
	}
public:
	template<typename ... Args>
	product* makeProduct(identifier id, Args&&...args) {
		std::lock_guard<std::mutex> lock(s_mutex);
		if (s_NewMethod.find(id) != s_NewMethod.end()) {
			constexpr std::size_t N = sizeof...(ctorTypes);
			constexpr std::size_t total_args = sizeof...(Args);
			auto all_args = std::forward_as_tuple(std::forward<Args>(args)...);
			constexpr size_t rest = total_args - N;

			auto ctor_args = TL_singleFactory::extract_front_impl(std::make_index_sequence<N>{}, all_args);
			auto init_args = TL_singleFactory::extract_back_impl<N>(std::make_index_sequence<rest>{}, all_args);

			std::unique_ptr<product> p(std::apply(s_NewMethod[id], ctor_args));

			if (!std::apply([this, id, p=p.get()](auto&&... init_args) {
				return this->init(id, p, std::forward<decltype(init_args)>(init_args)...);
				}, init_args)) {

				return nullptr;
			}

			return p.release(); ;
		}
		return nullptr;
	}

	template<typename ... Args>
	std::shared_ptr<product> makeShare(identifier id, Args&&...args) {
		std::lock_guard<std::mutex> lock(s_mutex);
		if (s_NewMethod.find(id) == s_NewMethod.end()) return nullptr;
		auto ss = TL_singleFactory::empty_type();
		return makeShareImpl(id, ss, std::forward<Args>(args)...);
	}

	template<typename ... Args>
	std::shared_ptr<product> makeShare_strong(identifier id, Args&&...args) {
		std::lock_guard<std::mutex> lock(s_mutex);
		if (s_NewMethod.find(id) == s_NewMethod.end()) return nullptr;
		return makeShareImpl(id, s_products_strong, std::forward<Args>(args)...);
	}

	template<typename ... Args>
	std::shared_ptr<product> makeShare_weak(identifier id, Args&&...args) {
		std::lock_guard<std::mutex> lock(s_mutex);
		if (s_NewMethod.find(id) == s_NewMethod.end()) return nullptr;
		return makeShareImpl(id, s_products_weak, std::forward<Args>(args)...);
	}

	template<typename ConcreateProduct, template<typename, typename>
	class policy = TL_singleFactory::singleFactory_createSelf
	>
	bool RegisterTofactory(identifier id, initFn f = {}) {
		std::lock_guard<std::mutex> lock(s_mutex);
		createFun fun = &policy<product, ConcreateProduct, ctorTypes...>::crateSelf;

		if (s_NewMethod.find(id) == s_NewMethod.end()) {
			s_NewMethod.insert(std::pair<identifier, createFun>(id, fun));
			s_initFns.insert(std::pair<identifier, initFn>(id, f));
			return true;
		}
		else {
			return false;
		}
	}
	
	bool UnRegisterTofactory(identifier id) {
		std::lock_guard<std::mutex> lock(s_mutex);
		if (s_NewMethod.find(id) != s_NewMethod.end()) {
			s_NewMethod.erase(id);
			s_initFns.erase(id);
			s_products_strong.erase(id);
			s_products_weak.erase(id);
			return true;
		}
		else {
			return false;
		}
	}
	
	std::list<identifier> getIdentifiers() {
		std::lock_guard<std::mutex> lock(s_mutex);
		std::list<identifier> keys;
		for (auto iter = s_NewMethod.begin(); iter != s_NewMethod.end(); iter++) {
			keys.push_back(iter->first);
		}
		return keys;
	}

	void releaseProducts() {
		std::lock_guard<std::mutex> lock(s_mutex);
		s_products_strong.clear();
		s_products_weak.clear();
	}
private:
	template<typename ... Args>
	bool init(identifier id, product* p, Args&&...values) {
		auto f = (s_initFns.find(id) != s_initFns.end()) ? s_initFns[id] : nullptr;
		if (!f)
			return true;
		try
		{
			f(p, std::forward<Args>(values)...);
			//((*p).*f)();
			return true;
		}
		catch (...)
		{
			return false;
		}
	}
private:
	std::map<identifier, initFn> s_initFns;
	std::map<identifier, product* (*)(ctorTypes...)> s_NewMethod;
	std::map<identifier, std::shared_ptr<product>> s_products_strong;
	std::map<identifier, std::weak_ptr<product>> s_products_weak;
	std::mutex s_mutex;
};

//template<typename ctorType>
//using IGPFactory = class SFactory<int, ctorType>;
//
//template<typename ctorType>
//using UGPFactory = class SFactory<unsigned int, ctorType>;
//
//template<typename ctorType>
//using SGPFactory = class SFactory<std::string, ctorType>;
#endif // SFACTORY_HPP