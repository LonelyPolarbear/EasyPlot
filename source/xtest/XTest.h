#include "xtestApi.h"
#include <memory>
#include <string>
#include <functional>

class XTestApp {
public:
    XTestApp(const std::string& prompt ="");
    ~XTestApp();

    int run();
    void addCmd(const std::string& cmd, const std::string& desc, const std::function<void()>& func);
    void setPrompt(const std::string& prompt);
protected:
    class Internal;
    std::shared_ptr<Internal> mData;
};