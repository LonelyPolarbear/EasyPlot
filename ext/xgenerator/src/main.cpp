#include "clang/Frontend/CompilerInstance.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/JSONCompilationDatabase.h"

#include "XASTFrontendAction.h"

#include <replxx.hxx>
#include <CLI/CLI.hpp>

#include <fstream>
#include <sstream>
#include <iostream>
#include <chrono>

#include <nlohmann/json.hpp>
#ifdef _WIN32
#define strncasecmp _strnicmp
#endif

//cmake -G "Visual Studio 17 2022" -Thost = x64 -DLLVM_ENABLE_PROJECTS = "clang;clang-tools-extra" -B build -S .
static CLI::App* app = nullptr;   // 全局指针，供补全回调访问

int utf8_str_len(const std::string& s) {
	int len = 0;
	for (size_t i = 0; i < s.size(); ++i) {
		if ((s[i] & 0xC0) != 0x80) ++len;
	}
	return len;
}

std::string getPrefix(const std::string& context) {
	//从前到后 数到前面空格开始 算本次的前缀 abc def,则前缀为 def
	int i = context.size() - 1;
	auto ss = context[i];
	while (i >= 0 && context[i] !=' ') --i;

	auto str =context.substr(i+1);
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

	// 4. 关键：计算共同前缀长度
	//std::string commonPrefix = prefix;
	//for (const auto& m : matches) {
	//	// 逐字符比较，找到共同前缀
	//	size_t len = 0;
	//	while (len < commonPrefix.size() && len < m.size() && commonPrefix[len] == m[len])
	//		++len;
	//	commonPrefix = commonPrefix.substr(0, len);
	//}

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

/*
 {
		"arguments": [
			"clang++",
			"-x",
			"c++-header",
			"-fsyntax-only",
			"E:\\work\\EasyPlot\\ext\\xgenerator\\output\\bin\\test.md"
		],
		"directory": "E:\\work\\EasyPlot\\ext\\xgenerator\\output\\bin",
		//directory解释
		1.
		字段指定了执行编译命令时的工作目录（即编译器进程的当前工作目录）。它的核心作用是解析编译命令中出现的相对路径
		如果编译命令中的某个参数（如源文件路径、-I 后面的头文件路径、-o 输出路径）是相对路径，Clang 会以 directory 为基准将其转换为绝对路径。
		例如："directory": "/home/user/project"，命令参数 -Iinclude 会被解释为 /home/user/project/include。
		如果命令中已经使用了绝对路径（如你的示例 E:\\work\\...），则 directory 不会被用于路径解析，但仍然需要存在。
		2.
		影响 Clang 内部的文件管理
		Clang 的 FileManager 和 SourceManager 会使用 directory 作为虚拟的当前工作目录，影响：
		__FILE__ 宏的展开结果（如果源文件路径是相对路径，则会拼接 directory）。
		诊断信息中显示的文件路径（相对路径会被转换为相对于 directory 的形式）。
		#include 搜索路径中的 "."（当前目录）的解析。

		"file": "E:\\work\\EasyPlot\\ext\\xgenerator\\output\\bin\\test.md"  
	}
*/
nlohmann::json create_json_command() {
	nlohmann::json json_command;
	json_command["directory"] = "";
	json_command["file"] = "";
	json_command["arguments"] = nlohmann::json::array();
	json_command["arguments"] += "clang++";
	json_command["arguments"] += "-std=c++17";
	json_command["arguments"] += "-x";
	json_command["arguments"] += "c++";
	json_command["arguments"] += "-fsyntax-only";
	//json_command["arguments"] += "-I";
	//json_command["arguments"] += "E:/work/EasyPlot/source";
	return json_command;
}

template<typename T>
CLI::Option* addOption(CLI::App* app,const std::string& option_name, const std::string& description, nlohmann::json& json_db, 
const std::string& json_key="",bool isAppend =false) {
	std::string real_key=json_key;
	if(json_key.empty()){
		real_key = option_name;
	}

	return app->add_option_function<T>(option_name, [&,real_key =real_key](const T& re)->void{
		json_db[real_key] = re;
		},description);
}

int main(int argc, const char** argv) {
	replxx::Replxx rx;
	std::vector<std::string> candidates;
	rx.set_completion_callback(std::bind(&hook_completion, std::placeholders::_1, std::placeholders::_2, std::cref(candidates), true));
	
	std::string currentExeDir = std::filesystem::path(argv[0]).parent_path().string();
	rx.history_load(currentExeDir+"\\history.txt");
	nlohmann::json json_command_template = create_json_command();							// 单个command的json模板
	nlohmann::json json_commands = nlohmann::json::array();											// 存放所有命令的json，对标compile_commands.json
	nlohmann::json json_outputConfig = nlohmann::json::object();										// 输出配置
	json_outputConfig["output_path"] = currentExeDir;														// 输出路径
	json_outputConfig["prefix"] = "pywrapper_";																	// 输出前缀
	json_outputConfig["suffix"] = "";																						// 输出后缀

	json_outputConfig["is_only_one_out_file"] = true;														//是否输出到一个文件
	json_outputConfig["only_one_out_file_name"] = "xtest";												//当json_outputConfig["is_only_one_out_file"]是true时候使用

	json_command_template["directory"]=currentExeDir;

	std::string filePath;
	std::string code;
	std::string input;

	app = new CLI::App();

	auto opt_file = app->add_option("-f,--file", "Input file names")->expected(1, 1000);
	auto opt_json_command_file = app->add_option("-j,--json", "Input json_commanf files")->expected(1);
	addOption<std::string>(app,"-o,--output", "output file path",json_outputConfig,"output_path")->expected(1);
	addOption<std::string>(app,"-p,--prefix", "output file prefix",json_outputConfig,"prefix")->expected(1);
	addOption<std::string>(app,"-s,--suffix", "output file suffix",json_outputConfig,"suffix")->expected(1);

	candidates.push_back("-o");
	candidates.push_back("--output");
	candidates.push_back("-p");
	candidates.push_back("--prefix");
	candidates.push_back("-s");
	candidates.push_back("--suffix");
	candidates.push_back("-f");
	candidates.push_back("--file");
	candidates.push_back("-j");
	candidates.push_back("--json");
	

	while (true) {
		input = rx.input(">>> ");

		if (input.empty()) continue;

		rx.history_add(input);

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

		if (opt_json_command_file->count() > 0) {
			std::unique_ptr<clang::tooling::CompilationDatabase> CompDB;
			std::string ErrorMsg;
			auto json_command_file = opt_json_command_file->as<std::string>();
			//如果json文件由外部提供，则不需要手动构建
			auto JSONDB = clang::tooling::JSONCompilationDatabase::loadFromFile(json_command_file, ErrorMsg, clang::tooling::JSONCommandLineSyntax::Windows);
			if (JSONDB) {
				CompDB = std::move(JSONDB);
				std::vector<std::string> allFiles = CompDB->getAllFiles();
				clang::tooling::ClangTool Tool(*CompDB, allFiles);
				Tool.run(new XASTFrontendActionFactory(json_outputConfig));
			}
			else {
				std::cout << "Error loading JSON Compilation Database: " << ErrorMsg << std::endl;
				continue;;
			}
		}else if(opt_file->count() > 0) {
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

			//如果json文件由外部提供，则不需要手动构建
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
	rx.history_save(currentExeDir + "\\history.txt");
	return 0;
}