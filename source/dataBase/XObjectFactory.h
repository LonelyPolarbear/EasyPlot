#pragma once
#include "lib00_utilty/gp/singlefactory.hpp"
#include "dataBaseApi.h"
#include "XDataBaseObject.h"
#include <string>

namespace XBaseObjectMeta {
	template<typename base, typename derived, typename... types>
	struct policyFactoryCreateDataobject {
		static std::shared_ptr<base> crateSelf(types... args) {
			return makeShareDbObject<derived>(args...);
		}
	};

	template class  DATABASE_API SFactory<std::string, XBaseObject(), void(), policyFactoryCreateDataobject>;
	using XObjectFactory = SFactory<std::string, XBaseObject(), void(), policyFactoryCreateDataobject>;

	template<typename T, typename Factory = XObjectFactory>
	void registerObject() {
		if constexpr (!std::is_abstract_v<T> && std::is_base_of_v<XBaseObject, T>) {
			Factory::Instance().RegisterTofactory<T>(T::className());
		}

		auto parentsNames = XQ_META::get_parent_className<T>();
		RegisterInherit(T::className(), parentsNames);
	}

	extern DATABASE_API void RegisterInherit(const std::string& child, std::vector<std::string>& parents);

	sptr<XBaseObject> DATABASE_API CreateObject(const std::string& className);

	extern DATABASE_API std::vector<std::string> GetParents(const std::string& className);

	extern DATABASE_API bool IsA(const std::string& className,const std::string& baseClassName);

	extern "C" {
		DATABASE_API void InitializeCoreTypes();
	}
}