#include "XShareVar.h"
#include <nlohmann/json.hpp>

class XShareVar::Internal {
public:
	nlohmann::json mJson;
	Internal() {
		mJson["cfg1"]=1;
		mJson["cfg2"]="hello";
	}
};

void XShareVar::load(const std::string& path)
{

}

nlohmann::json& XShareVar::getJson()
{
	return mData->mJson;
}

XShareVar::XShareVar():mData(new Internal)
{
}
