#pragma once
#include "xtestApi.h"
#include <memory>
#include <string>
#include <functional>

class XTestApp {
public:
    XTestApp(const std::string& name="", const std::string& desc="");
    ~XTestApp();

    int run();
    XTestApp* addCmd(const std::string& cmd, const std::string& desc, const std::function<void()>& func);
	XTestApp* setName(const std::string& name);
	XTestApp* setDesc(const std::string& desc);
protected:
    class Internal;
    std::shared_ptr<Internal> mData;
};