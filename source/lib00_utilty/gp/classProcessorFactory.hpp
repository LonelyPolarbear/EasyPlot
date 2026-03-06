#pragma once
#include <functional>
#include <unordered_map>
#include <string>
#include <exception>
#include <stdexcept>

//工厂，根据单元类类型注册不同的处理函数
template<unsigned int N, typename Fn>
class classProcessorFactory;		//根据class类名调用不同的版本

template<unsigned int N, typename result_t, typename ...Args>
class classProcessorFactory<N,result_t(Args...)> {
public:
	using ProcessFunction = std::function<result_t(Args...)>;
private:
	classProcessorFactory() = default;
public:
	static classProcessorFactory& instance() {
		static classProcessorFactory factory;
		return factory;
	}
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

#define CLASS_PROCESSOR_FACTORY(Signature) \
    classProcessorFactory<__COUNTER__, Signature>