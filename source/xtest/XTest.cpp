#include "XTest.h"
#include <windows.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <functional>

//#include <linenoise/linenoise.h>
#include <replxx.hxx>
#include <CLI/CLI.hpp>

#include <nlohmann/json.hpp>
#ifdef _WIN32
#define strncasecmp _strnicmp
#endif

struct CLI_cmd_data {
	std::string cmd;
	std::string cmdDesc;
	std::function<void()> fn;
};

std::string getPrefix(const std::string& context) {
	//从前到后 数到前面空格开始 算本次的前缀 abc def,则前缀为 def
	int i = context.size() - 1;
	auto ss = context[i];
	while (i >= 0 && context[i] != ' ') --i;

	auto str = context.substr(i + 1);
	return str;
}

replxx::Replxx::completions_t hook_completion(std::string const& context, int& contextLen,
	std::vector<std::string> const& examples,
	bool ignoreCase) {
	replxx::Replxx::completions_t completions;

	std::string prefix = getPrefix(context);
	int prefixLen = prefix.size();

	// 3. 收集匹配的补全候选（忽略大小写）
	std::vector<std::string> matches;
	for (const auto& e : examples) {
		bool match = ignoreCase
			? (strncasecmp(e.c_str(), prefix.c_str(), prefix.size()) == 0)
			: (e.compare(0, prefix.size(), prefix) == 0);
		if (match) {
			matches.push_back(e);
		}
	}

	if (matches.empty()) {
		return completions;  // 无匹配
	}

	// 构建补全列表
	if (matches.size() > 1) {
		for (const auto& m : matches) {
			replxx::Replxx::Color color = replxx::Replxx::Color::RED;
			completions.emplace_back(m.c_str(), color);
		}
	}
	else {
		contextLen = prefixLen;		//replxx 会删除光标前的几个字符
		completions.emplace_back(matches[0], replxx::Replxx::Color::DEFAULT);
	}
	return completions;
}

class XTestApp::Internal {
public:
	Internal() {
		app = new CLI::App;
	}
	~Internal() {
		delete app;
	}
public:
	std::string input;
	std::map<std::string, CLI_cmd_data> cmdCallbacks;
	CLI::App* app = nullptr;
	std::vector<XTestApp*> subApps;
	replxx::Replxx rx;
	std::vector<std::string> candidates;

protected:
	bool isNew = true;
};

XTestApp::XTestApp(const std::string& name,const std::string& desc ):mData(new Internal())
{
	mData->rx.set_completion_callback(std::bind(&hook_completion, std::placeholders::_1, std::placeholders::_2, std::cref(mData->candidates), true));
	mData->app->name(name)->description(desc);
}

XTestApp::~XTestApp()
{
}

int XTestApp::run()
{
	while (true) {
		mData->input = mData->rx.input(">>> ");
		if (mData->input.empty()) continue;

		mData->rx.history_add(mData->input);

		if (mData->input == "exit")
			break;
		std::istringstream iss(mData->input);
		std::vector<std::string> args{ std::istream_iterator<std::string>(iss),std::istream_iterator<std::string>() };

		
		try {
			mData->app->parse(args);
		}
		catch (const CLI::ParseError& e) {
			if (e.get_exit_code() != 0) {
				std::cerr << e.what() << "\n";
			}
			else {
				std::cout << mData->app->help() << std::endl;
			}
		}
	}
	return 1;
}

XTestApp* XTestApp::addCmd(const std::string& cmd, const std::string& desc, const std::function<void()>& func)
{
	mData->cmdCallbacks[cmd] = CLI_cmd_data{ cmd,desc,func };
	mData->candidates.push_back(cmd);
	auto sub_cli_app = mData->app->add_subcommand(cmd, desc)->callback(func);
	return this;
}

XTestApp* XTestApp::setName(const std::string& name)
{
	mData->app->name(name);
	return this;
}

XTestApp* XTestApp::setDesc(const std::string& desc)
{
	mData->app->description(desc);
	return this;
}
