#include "XPythonConsole.h"
#include <linenoise/linenoise.h>
#include <CLI/CLI.hpp>
#include "xpython/XPython.h"
#include <pybind11/pybind11.h>
#include <pybind11/embed.h>
#include <thread>
static std::string toLower(const std::string& s) {
	std::string result;
	result.reserve(s.size());
	for (unsigned char c : s) {
		result.push_back(static_cast<char>(std::tolower(c)));
	}
	return result;
}

static void completionHook(const char* buf, linenoiseCompletions* lc, void* userdata) {
	//std::map<std::string, CLI_cmd_data>* cmdCallbacks = (std::map<std::string, CLI_cmd_data>*)userdata;
	std::string input(buf);
	std::string inputLower = toLower(input);
	//std::string inputLower = toLower(input);
	/*for (auto& cmd : *cmdCallbacks) {
		std::string cmdLower = toLower(cmd.first);
		if (cmdLower.find(inputLower) == 0) {
			linenoiseAddCompletion(lc, cmd.first.c_str());
		}
	}*/
}

class XPythonConsole::Internal {
public:
	Internal() {
		app = new CLI::App;
		app->add_option("code", code)->required();
	}
	~Internal() {
		delete app;
	}
	char* line = nullptr;
	CLI::App *app = nullptr;
	std::string code;
	std::thread thrd;
	bool running = true;
};

XPythonConsole::XPythonConsole():mData(new Internal)
{
	//linenoiseSetCompletionCallback(completionHook, &mData->cmdCallbacks);
}

XPythonConsole::~XPythonConsole()
{
	stop();
}


void XPythonConsole::run()
{
	mData->thrd =std::thread (&XPythonConsole::runImpl, this);
}

void XPythonConsole::stop()
{
	mData->running = false;
	if (mData->thrd.joinable()) {
		mData->thrd.join();
	}
}

int XPythonConsole::runImpl()
{
	//调用 PyGILState_Ensure() 获取 GIL。如果当前线程尚未持有 GIL，则阻塞直到成功获取；如果已经持有，则增加内部递归计数（不会死锁
	//析构时：调用 PyGILState_Release() 释放 GIL（递归释放）。
	while (mData->running) {
		std::cout << ">>> ";
		mData->line = linenoise(">>> ");
		if (mData->line == nullptr) break;

		std::string input(mData->line);

		//if (!std::getline(std::cin, line) || line == "exit") break;
		if (input.empty()) continue;

		linenoiseHistoryAdd(input.c_str());
		//linenoiseHistorySave("history.txt"); // 保存历史记录

		if (input == "exit")
			break;
		std::istringstream iss(input);
		std::vector<std::string> args{ std::istream_iterator<std::string>(iss),std::istream_iterator<std::string>() };

		try {
			mData->code.clear();
			mData->app->parse(args);
		}
		catch (const CLI::ParseError& e) {
			if (e.get_exit_code() != 0) {
				std::cerr << e.what() << "\n";
			}
			else {
				std::cout << mData->app->help() << std::endl;
			}
			continue;
		}

		if (!mData->code.empty()) {
			auto ins = XPython::Instance();
			ins->execute(mData->code);
		}
	}
	return 1;
	//pybind11::gil_scoped_release release;
}
