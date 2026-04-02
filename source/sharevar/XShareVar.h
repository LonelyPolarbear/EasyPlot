#pragma once
#include "sharevarApi.h"
#include <string>
#include <memory>
#include <nlohmann/json.hpp>

class SHAREVAR_API XShareVar {
public:
	static XShareVar& instance() {
		static XShareVar var;
		return var;
	}

	std::string currentExeDir;
public:
	void load(const std::string& path);
	nlohmann::json& getJson();
protected:
	XShareVar();
protected:
	class Internal;
	std::shared_ptr<Internal> mData;
};