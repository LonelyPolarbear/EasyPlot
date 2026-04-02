#include "clang/Frontend/CompilerInstance.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/JSONCompilationDatabase.h"

#include "XASTFrontendAction.h"
#include <linenoise/linenoise.h>
#include <CLI/CLI.hpp>

#include <fstream>
#include <sstream>
#include <iostream>

#include <nlohmann/json.hpp>

//cmake -G "Visual Studio 17 2022" -Thost = x64 -DLLVM_ENABLE_PROJECTS = "clang;clang-tools-extra" -B build -S .
static CLI::App* app = nullptr;   // 全局指针，供补全回调访问
static std::string g_prompt = ">>> ";

static std::string toLower(const std::string& s) {
	std::string result;
	result.reserve(s.size());
	for (unsigned char c : s) {
		result.push_back(static_cast<char>(std::tolower(c)));
	}
	return result;
}
// 补全回调函数
static void completionHook(const char* buf, linenoiseCompletions* lc, void* userdata) {
	if (!app) return;
	std::string input(buf);
	std::vector<std::string> matches;

	// 收集所有选项名称（短名带 -，长名带 --）
	for (auto* opt : app->get_options()) {
		for (const auto& sn : opt->get_snames()) {
			std::string full = "-" + sn;
			if (full.find(input) == 0) matches.push_back(full);
		}
		for (const auto& ln : opt->get_lnames()) {
			std::string full = "--" + ln;
			if (full.find(input) == 0) matches.push_back(full);
		}
	}

	// 如果匹配项多于 1 个，打印列表并刷新
	if (matches.size() > 1) {
		std::cerr << std::endl;
		for (const auto& m : matches) {
			std::cerr << "  " << m << std::endl;
		}
		std::cout << g_prompt;
		return;
	}

	if (matches.size() == 1) {
		linenoiseAddCompletion(lc, matches[0].c_str());
	}
}

static llvm::cl::OptionCategory MyToolCategory("My Tool Options");

nlohmann::json create_json_command() {
	nlohmann::json json_command;
	json_command["directory"] = "";
	json_command["file"] = "";
	json_command["arguments"] = nlohmann::json::array();
	json_command["arguments"] += "clang++";
	json_command["arguments"] += "-x";
	json_command["arguments"] += "c++-header";
	json_command["arguments"] += "-fsyntax-only";
	return json_command;
}

template<typename T>
CLI::Option* addOption(CLI::App* app,const std::string& option_name, const std::string& description, nlohmann::json& json_db, const std::string& json_key="") {
	std::string real_key=json_key;
	if(json_key.empty()){
		real_key = option_name;
	}

	return app->add_option_function<T>(option_name, [&,real_key =real_key](const T& re)->void{
		json_db[real_key] = re;
		},description);
}

int main(int argc, const char** argv) {
	std::string currentExeDir = std::filesystem::path(argv[0]).parent_path().string();
	nlohmann::json json_command_template = create_json_command();							// 单个command的json模板
	nlohmann::json json_commands = nlohmann::json::array();											// 存放所有命令的json，对标compile_commands.json
	nlohmann::json json_outputConfig = nlohmann::json::object();										// 输出配置
	json_outputConfig["output_path"] = currentExeDir;														// 输出路径
	json_outputConfig["prefix"] = "pywrapper_";																	// 输出前缀
	json_outputConfig["suffix"] = "";																						// 输出后缀

	json_command_template["directory"]=currentExeDir;

	linenoiseSetCompletionCallback(completionHook, nullptr);
	linenoiseHistoryLoad("history.txt");

	char* line = nullptr;
	std::string filePath;
	std::string code;

	app = new CLI::App();

	auto opt_file = app->add_option("-f,--file", "Input file names")->expected(1, 1000);
	//auto opt_out_path = app->add_option("-o,--output", "output file path")->expected(1);
	addOption<std::string>(app,"-o,--output", "output file path",json_outputConfig,"output_path")->expected(1);
	addOption<std::string>(app,"-p,--prefix", "output file prefix",json_outputConfig,"prefix")->expected(1);
	addOption<std::string>(app,"-s,--suffix", "output file suffix",json_outputConfig,"suffix")->expected(1);

	while (true) {
		std::cout << ">>> ";
		line = linenoise(">>> ");
		if (line == nullptr) break;

		std::string input(line);

		if (input.empty()) continue;

		linenoiseHistoryAdd(input.c_str());
		linenoiseHistorySave("history.txt"); // 保存历史记录

		if (input == "exit")
			break;
		std::istringstream iss(input);
		std::vector<std::string> args{ std::istream_iterator<std::string>(iss),std::istream_iterator<std::string>() };
		std::reverse(args.begin(), args.end());
		try {
			app->parse(args);
		}
		catch (const CLI::ParseError& e) {
			if (e.get_exit_code() != 0) {
				std::cerr << e.what() << "\n";
			}
			else {
				std::cout << app->help() << std::endl;
			}
			continue;
		}

		if (opt_file->count() > 0) {
			auto inputFiles = opt_file->as<std::vector<std::string>>();

			for (auto& file : inputFiles) {
				nlohmann::json json_command = json_command_template;
				json_command["file"] = currentExeDir + "\\" + file;
				json_command["arguments"] += currentExeDir + "\\" + file;
				json_commands.push_back(json_command);
			}
			std::cout<<"===============================================================>\n";
			std::cout << json_commands.dump(4) << std::endl;
			std::cout << "===============================================================>\n";

			std::unique_ptr<clang::tooling::CompilationDatabase> CompDB;
			std::string ErrorMsg;

			auto json_str =json_commands.dump();
			auto JSONDB = clang::tooling::JSONCompilationDatabase::loadFromBuffer(json_str, ErrorMsg, clang::tooling::JSONCommandLineSyntax::Windows);
			if (JSONDB) {
				CompDB = std::move(JSONDB);
				std::vector<std::string> allFiles = CompDB->getAllFiles();
				clang::tooling::ClangTool Tool(*CompDB, allFiles);
				Tool.run(new XASTFrontendActionFactory(json_outputConfig));
			}
			else {
				std::cout << "Error loading JSON Compilation Database: " << ErrorMsg << std::endl;
				break;
			}
		}
	}

	delete app;
	return 0;
}