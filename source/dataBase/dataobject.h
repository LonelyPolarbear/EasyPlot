#pragma once

#include "databaseApi.h"
#include "XTimeStamp.h"
#include <memory>
#include <map>
#include <functional>

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


template<typename object>
struct dataObjectShare:public object {
protected:
	dataObjectShare() :object() {}
public:
	// 静态工厂函数使用完美转发
	static  std::shared_ptr<object> Create() {
		std::shared_ptr<object> p;
		p.reset( new dataObjectShare<object>());
		return p;
	}
};

template<typename object>
std::shared_ptr<object> makeShareDbObject() {
	return dataObjectShare<object>::Create();
}

class database_API DataBaseObject : public std::enable_shared_from_this<DataBaseObject> {
protected:
	DataBaseObject() ;
	virtual ~DataBaseObject();
	XTimeStamp m_DataModifyTime;

public:
	void Modified() {
		m_DataModifyTime.Modified();
		for (auto observer : observers) {
			auto p = observer.first;
			if (auto ptr = p.lock()) {
				observer.second(ptr);
			}
		}

		//弱引用的销毁
		auto it = observers.begin();
		while (it != observers.end()) {
			if (it->first.expired()) {
				it = observers.erase(it);
			}
			else {
				++it;
			}
		}
	}

	bool addObserver(std::shared_ptr<DataBaseObject> obj, std::function<void(std::shared_ptr<DataBaseObject>)> f) {
		observers.insert(std::make_pair(std::weak_ptr<DataBaseObject>(obj), f));
		return false;
	}

	bool removeObserver(std::shared_ptr<DataBaseObject> obj) {
		auto it = observers.begin();
		while (it != observers.end()) {
			if (it->first.lock() == obj) {
				it = observers.erase(it);
				return true;
			}
			else {
				++it;
			}
		}
		return false;
	}

	XTimeStamp GetTimeStamp() const {
		return m_DataModifyTime;
	}

	std::map<std::weak_ptr<DataBaseObject>,std::function<void(std::shared_ptr<DataBaseObject>)>,std::owner_less<std::weak_ptr<DataBaseObject>>> observers;
};
