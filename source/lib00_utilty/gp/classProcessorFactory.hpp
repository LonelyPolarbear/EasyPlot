#pragma once
#include <functional>
#include <unordered_map>
#include <string>
#include <exception>
#include <stdexcept>

//工厂，根据单元类类型注册不同的处理函数
template<typename Fn>
class classProcessorFactory;

template<typename result_t, typename ...Args>
class classProcessorFactory<result_t(Args...)> {
public:
	using ProcessFunction = std::function<result_t(Args...)>;
public:
	classProcessorFactory() = default;
public:
	/*static classProcessorFactory& instance() {
		static classProcessorFactory factory;
		return factory;
	}*/
public:
	void registerProcessor(const std::string& name, ProcessFunction Fn) {
		processors_[name] = Fn;
	}

	result_t process(const std::string& name, Args... args) {
		auto it = processors_.find(name);
		if (it != processors_.end()) {
			return processors_[name](args...);
		}
		else {
			throw std::runtime_error("Unknown element type: " + name);
			//std::cout<< "Unknown element type: " + name<<std::endl;
		}
	}
	bool hasProcessor(const std::string& name) const {
		return processors_.find(name)!= processors_.end();
	}
private:
	std::unordered_map<std::string, ProcessFunction> processors_;
};

template<typename T>
struct map_fun_type;

template<typename result_t, typename ...Args>
struct map_fun_type<result_t(Args...)>{
	using type = std::unordered_map<std::string, std::function<result_t(Args...)>>;
	using result_type = result_t;
};

template<typename F1,typename...FS>
class classMultiProcessorFactory{
	using myTuple = std::tuple<typename map_fun_type<F1>::type, typename map_fun_type<FS>::type...>;
	using myTupleFn = std::tuple<std::function<F1>, std::function<FS>...>;
	using myTupleFnResult = std::tuple<typename map_fun_type<F1>::result_type, typename map_fun_type<FS>::result_type...>;

public:
	classMultiProcessorFactory() = default;

	template<unsigned int IDX>
	void registerProcessor(const std::string& name, const std::tuple_element_t<IDX, myTupleFn> &F) {
		auto& processors = std::get<IDX>(processorsTuple);
		processors[name] = F;
	}

	template<unsigned int IDX>
	bool hasProcessor(const std::string& name) const {
		auto &processors = std::get<IDX>(processorsTuple);
		return processors.find(name) != processors.end();
	}

	template<unsigned int IDX,typename...Args>
	decltype(auto) process(const std::string& name, Args&&... args) {
		auto& processors = std::get<IDX>(processorsTuple);
		auto it = processors.find(name);
		if (it != processors.end()) {
			return processors[name](std::forward<Args>(args)...);
		}
		else {
			throw std::runtime_error("Unknown element type: " + name);
		}
	}
public:
	myTuple processorsTuple;
};