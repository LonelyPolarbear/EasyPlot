#include "XTest.h"
#include <windows.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <functional>

#include <linenoise/linenoise.h>
#include <CLI/CLI.hpp>

// 将字符串转换为小写（注意处理 unsigned char 避免断言）
static std::string toLower(const std::string& s) {
	std::string result;
	result.reserve(s.size());
	for (unsigned char c : s) {
		result.push_back(static_cast<char>(std::tolower(c)));
	}
	return result;
}

struct CLI_cmd_data {
	std::string cmd;
	std::string cmdDesc;
	std::function<void()> fn;
};

void completionHook(const char* buf, linenoiseCompletions* lc, void* userdata) {
	std::map<std::string, CLI_cmd_data> *cmdCallbacks = (std::map<std::string, CLI_cmd_data>*)userdata;
	std::string input(buf);
	std::string inputLower = toLower(input);
	//std::string inputLower = toLower(input);
	for (auto & cmd : *cmdCallbacks) {
		std::string cmdLower = toLower(cmd.first);
		if (cmdLower.find(inputLower) == 0) {
			linenoiseAddCompletion(lc, cmd.first.c_str());
		}
	}
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
	char *line = nullptr;
	std::map<std::string, CLI_cmd_data> cmdCallbacks;
	CLI::App* app = nullptr;
	std::vector<XTestApp*> subApps;

protected:
	bool isNew = true;
};

XTestApp::XTestApp(const std::string& name,const std::string& desc ):mData(new Internal())
{
	linenoiseSetCompletionCallback(completionHook, &mData->cmdCallbacks);
	mData->app->name(name)->description(desc);
}

XTestApp::~XTestApp()
{
}

int XTestApp::run()
{
	//for (auto d : mData->cmdCallbacks) {
		//app.add_subcommand(d.first, d.second.cmdDesc)->callback(d.second.fn);
	//}

	/*
	bool flag = false;
	app.add_flag("-f,--flag", flag, "这是一个标志选项");

	int number = 0;
	app.add_option("-n,--number", number, "一个整数参数");

	std::string name = "default";
	app.add_option("--name", name, "你的名字")->required();  // 带这个关键字说明必须有
	*/
	while (true) {
		std::cout << "> ";
		mData->line = linenoise("> ");
		if (mData->line == nullptr) break;

		std::string input(mData->line);

		//if (!std::getline(std::cin, line) || line == "exit") break;
		if (input.empty()) continue;

		linenoiseHistoryAdd(input.c_str());
		linenoiseHistorySave("history.txt"); // 保存历史记录

		if(input =="exit")
			break;
		std::istringstream iss(input);
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
