/**
* @file         AbstractFactory.hpp
* @brief       泛型抽象工厂模板类，实现泛型抽象工厂模式
* @author     宋伟军
* @date        2025.6.27
* @license Copyright(c)2023, 英特工程仿真技术(大连)有限公司版权所有
*/

#ifndef __ABSTRACTFACTORY__HPP
#define __ABSTRACTFACTORY__HPP

#include <map>
#include <list>
#include <memory>
#include <mutex>

template<typename ...>
struct tmpTypelist{};          

template<typename base,typename object,typename TList>
struct Factory_createSelf;

template<typename base,typename object,typename... types>
struct Factory_createSelf<base,object,tmpTypelist<types...>>:public object{
    Factory_createSelf(types... args):object(args...){}
    static base*crateSelf(types... values){
        return new Factory_createSelf<base,object,tmpTypelist<types...>>(values...); 
    }
};


template<unsigned int type>
struct IntToType{enum {val = type};};

template<unsigned int N>
struct abstractfactory_flag{
    inline static const IntToType<0> p1;
    inline static const IntToType<1> p2;
    inline static const IntToType<2> p3;
    inline static const IntToType<3> p4;
    inline static const IntToType<4> p5;
    inline static const IntToType<5> p6;
    inline static const IntToType<6> p7;
    inline static const IntToType<7> p8;
    inline static const IntToType<8> p9;
    inline static const IntToType<9> p10;
};

template<typename T>
struct ConvertFun2FunPointer;

template<typename FUN,typename ...Args>
struct ConvertFun2FunPointer<FUN(Args...)>
{
	using pointerType = FUN*(*)(Args...);
    using product = FUN;
    using TList = tmpTypelist<Args...>;
};

//抽象工厂模式
template<typename identifier,unsigned int type,typename ctorType,typename ... ctorTypes>
struct GenScatterHierarchy:public GenScatterHierarchy<identifier,type+1,ctorTypes...>
{
    using pointerType =typename ConvertFun2FunPointer<ctorType>::pointerType;
    using product = typename ConvertFun2FunPointer<ctorType>::product;
    using ctor_types = typename ConvertFun2FunPointer<ctorType>::TList;

	using GenScatterHierarchy<identifier, type + 1, ctorTypes...>::DoCreate;                       //跨域重载
	using GenScatterHierarchy<identifier, type + 1, ctorTypes...>::DoCreateShare;             //跨域重载
    using GenScatterHierarchy<identifier,type+1,ctorTypes...>::RegisterTofactory;            //跨域重载
    using GenScatterHierarchy<identifier,type+1,ctorTypes...>::UnRegisterTofactory;       //跨域重载
    using GenScatterHierarchy<identifier,type+1,ctorTypes...>::getIdentifiers;                  //跨域重载
    
    template<typename ... Args>
     product* DoCreate(IntToType<type>,identifier id,Args...values){
        if(AssoicMap.find(id) != AssoicMap.end()){
            return AssoicMap[id](values...);
        }
        return nullptr;
    }

	template<typename ... Args>
	 std::shared_ptr<product> DoCreateShare(IntToType<type>, identifier id, Args...values) {
        std::lock_guard<std::mutex> lock(s_mutex);
		if (AssoicMap.find(id) != AssoicMap.end()) {
			product* p = AssoicMap[id](values...);
			auto result = std::shared_ptr<product>(p);
			auto f = s_initFns[id];
			if (f != nullptr) {
				((*p).*f)();
			}
			return result;
		}
		return std::shared_ptr<product>(nullptr);
	}

    template<typename ConcreateProduct,template<typename,typename,typename>
                                                                    class policy = Factory_createSelf
                    >
     bool RegisterTofactory(IntToType<type>,identifier id, void(product::* f)() = nullptr){
        std::lock_guard<std::mutex> lock(s_mutex);
        pointerType fun = &Factory_createSelf<product,ConcreateProduct,ctor_types>::crateSelf;
        if(AssoicMap.find(id) == AssoicMap.end()){
            AssoicMap.insert(std::pair<identifier,pointerType>(id,fun));
            s_initFns.insert(std::pair<identifier, void(product::*)()>(id, f));
            return true;
        }else{
            return false;
        }
    }

     bool UnRegisterTofactory(IntToType<type>,identifier id,pointerType fun){
        std::lock_guard<std::mutex> lock(s_mutex);
        if(AssoicMap.find(id) != AssoicMap.end()){
            AssoicMap.erase(id);
			if (s_initFns.find(id) != s_initFns.end()) {
				s_initFns.erase(id);
			}
            return true;
        }else{
            return false;
        }
    }

     std::list<identifier> getIdentifiers(IntToType<type>){
        std::lock_guard<std::mutex> lock(s_mutex);
        std::list<identifier> keys;
        for(auto iter = AssoicMap.begin();iter != AssoicMap.end();iter++){
            keys.push_back(iter->first);
        }
        return keys;
    }
public:
    std::map<identifier,pointerType> AssoicMap;
     std::map<identifier, void(product::*)()> s_initFns;
     std::mutex s_mutex;
};

template<typename identifier,unsigned int type,typename ctorType>
struct GenScatterHierarchy<identifier,type,ctorType>
{
    using pointerType =typename ConvertFun2FunPointer<ctorType>::pointerType;
    using product = typename ConvertFun2FunPointer<ctorType>::product;
    using ctor_types = typename ConvertFun2FunPointer<ctorType>::TList;
    
    template<typename ... Args>
     product* DoCreate(IntToType<type>,identifier id,Args...values){
        std::lock_guard<std::mutex> lock(s_mutex);
        if(AssoicMap.find(id) != AssoicMap.end()){
            product* p = AssoicMap[id](values...);
			auto f = s_initFns[id];
			if (f != nullptr) {
				((*p).*f)();
			}
            return p;
        }
        return nullptr;
    }

	template<typename ... Args>
	 std::shared_ptr<product> DoCreateShare(IntToType<type>, identifier id, Args...values) {
        std::lock_guard<std::mutex> lock(s_mutex);
		if (AssoicMap.find(id) != AssoicMap.end()) {
            product* p = AssoicMap[id](values...);
			auto result = std::shared_ptr<product>(p);
            auto f = s_initFns[id];
			if (f != nullptr) {
				((*p).*f)();
			}
            return result;
		}
		return std::shared_ptr<product>(nullptr);
	}

    template<typename ConcreateProduct,template<typename,typename,typename>
                                                                    class policy = Factory_createSelf
                    >
     bool RegisterTofactory(IntToType<type>,identifier id, void(product::* f)() = nullptr){
        std::lock_guard<std::mutex> lock(s_mutex);
        pointerType fun = &Factory_createSelf<product,ConcreateProduct,ctor_types>::crateSelf;
        if(AssoicMap.find(id) == AssoicMap.end()){
            AssoicMap.insert(std::pair<identifier,pointerType>(id,fun));
            s_initFns.insert(std::pair<identifier, void(product::*)()>(id, f));
            return true;
        }else{
            return false;
        }
    }

     bool UnRegisterTofactory(IntToType<type>,identifier id){
        std::lock_guard<std::mutex> lock(s_mutex);
        if(AssoicMap.find(id) != AssoicMap.end()){
            AssoicMap.erase(id);
			if (s_initFns.find(id) != s_initFns.end()) {
				s_initFns.erase(id);
			}
            return true;
        }else{
            return false;
        }
    }

     std::list<identifier> getIdentifiers(IntToType<type>){
        std::lock_guard<std::mutex> lock(s_mutex);
        std::list<identifier> keys;
        for(auto iter = AssoicMap.begin();iter != AssoicMap.end();iter++){
            keys.push_back(iter->first);
        }
        return keys;
    }
public:
     std::map<identifier,pointerType> AssoicMap;
     std::map<identifier, void(product::*)()> s_initFns;
     std::mutex s_mutex;
};

template<typename identifier,typename ... ctorTypes>
class AbstractFactory:public GenScatterHierarchy<identifier,0,ctorTypes...>{
public:
    static AbstractFactory& Instance() {
        static AbstractFactory instance;
        return instance;
     }
};

template<typename ...ctorTypes>
using IGPAbsFactory = class AbstractFactory<int,ctorTypes...>;  

template<typename ...ctorTypes>
using UGPAbsFactory = class AbstractFactory<unsigned int,ctorTypes...>;  

template<typename ...ctorTypes>
using SGPAbsFactory = class AbstractFactory<std::string,ctorTypes...>;  

#endif