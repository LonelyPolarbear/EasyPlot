
#include <replxx.hxx>
#include <CLI/CLI.hpp>

#include <fstream>
#include <sstream>
#include <iostream>
#include <chrono>
#include <regex>
#include <iomanip>   
#include <filesystem>
#include <thread>

#include <nlohmann/json.hpp>
#ifdef _WIN32
#define strncasecmp _strnicmp
#endif


static CLI::App* app = nullptr;   // 全局指针，供补全回调访问

std::string getPrefix(const std::string& context) {
	//从前到后 数到前面空格开始 算本次的前缀 abc def,则前缀为 def
	int i = context.size() - 1;
	auto ss = context[i];
	while (i >= 0 && context[i] != ' ') --i;

	auto str = context.substr(i + 1);
	return str;
}

static std::vector<std::string> read_line_include_h(const std::string& path) {
	std::vector<std::string> lines;
	std::ifstream stm;
	stm.open(path);
	std::string line;
	std::regex reg(R"(\s*#\s*include\s*["<]([^">]+)[">])");
	while (std::getline(stm,line))
	{
		std::smatch match;
		if (std::regex_match(line, match, reg)) {
			lines.push_back(match[1].str());
		}
	}
	return lines;
}

std::vector<std::string> splitByPathSeparator(const std::string& str) {
	std::vector<std::string> result;
	size_t start = 0;
	size_t pos1 = str.find('\\'); // 查找反斜杠
	size_t pos2 = str.find('/');  // 查找正斜杠

	while (pos1 != std::string::npos || pos2 != std::string::npos) {
		// 取两个分隔符中位置更小的那个（即先出现的）
		size_t pos = std::string::npos;
		if (pos1 == std::string::npos) {
			pos = pos2;
		}
		else if (pos2 == std::string::npos) {
			pos = pos1;
		}
		else {
			pos = std::min(pos1, pos2);
		}

		result.push_back(str.substr(start, pos - start));
		start = pos + 1;

		// 更新两个分隔符的位置（从 start 开始继续查找）
		pos1 = str.find('\\', start);
		pos2 = str.find('/', start);
	}

	// 添加最后一段
	result.push_back(str.substr(start));
	return result;
}


std::vector<std::filesystem::path> GetAbsPath(const std::filesystem::path& base_path, std::vector<std::string> all_include_files) {
	std::vector<std::filesystem::path> all_path;
	
	//扫描每一行，获取路径
	for (auto &s : all_include_files) {
		auto relative_path = splitByPathSeparator(s);
		
		std::filesystem::path cur_path = base_path;
		for(auto &front:relative_path)
		{
			if (front == ".") {
				continue;
			}else if(front =="..")
				cur_path = cur_path.parent_path();
			else {
				cur_path /= front;
			}
		}

		all_path.push_back(cur_path);
	}
	return all_path;

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


bool ScanFile(const std::string& json_command_file, const std::string& scan_file_h, const std::string& out_put_json);

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
	std::string ss;
	bool runLoop = true;
	if (argc > 1) {
		//说明是外部启动，此时不需要启动内循环
		runLoop = false;
	}
	replxx::Replxx rx;
	std::vector<std::string> candidates;
	rx.set_completion_callback(std::bind(&hook_completion, std::placeholders::_1, std::placeholders::_2, std::cref(candidates), true));
	
	std::string currentExeDir = std::filesystem::path(argv[0]).parent_path().string();
	rx.history_load(currentExeDir+"\\history2.txt");
	
	nlohmann::json config;
	config["scan_file"]="";
	config["json_file"] = "";
	config["out_put_json"] = "";		//输出的json文件路径，此处使用绝对路径

	std::string filePath;
	std::string code;
	std::string input;

	app = new CLI::App();

	auto json_command =addOption<std::string>(app, "--json", "Input json_commanf files", config, "json_file")->expected(1)->required(true);
	auto scan_command = addOption<std::string>(app, "--scan", "input scan file ", config, "scan_file")->expected(1)->required(true);
	auto out_put_json = addOption<std::string>(app, "--out", "out put json file ", config, "out_put_json")->expected(1)->required(true);

	candidates.push_back("--json");
	candidates.push_back("--scan");
	candidates.push_back("--out");
	
	do{
		std::vector<std::string> args;

		if (runLoop) {
			input = rx.input(">>> ");

			if (input.empty()) continue;

			rx.history_add(input);

			if (input == "exit")
				break;
			std::istringstream iss(input);
			args= std::vector<std::string>{ std::istream_iterator<std::string>(iss),std::istream_iterator<std::string>() };
			std::reverse(args.begin(), args.end());
		}
		
		try {
			if (runLoop) {
				app->parse(args);
			}
			else {
				CLI11_PARSE(*app, argc, argv);
			}
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

		std::cout << config.dump(4) << std::endl;
		if (json_command->count() > 0 && scan_command->count() > 0) {
			std::string  json_command_file = config["json_file"];
			std::string  scan_file = config["scan_file"];
			std::string  out_put_json = config["out_put_json"];

			if (ScanFile(json_command_file, scan_file, out_put_json)) {
				std::cout << "success" << std::endl;
			}
		}
	} while (runLoop);

	delete app;
	rx.history_save(currentExeDir + "\\history2.txt");
	return 0;
}

bool is_parent_of(const std::filesystem::path& parent, const std::filesystem::path& child) {
	// 1. 先规范化路径，解析掉 . 和 ..
	auto norm_parent = std::filesystem::weakly_canonical(parent);
	auto norm_child = std::filesystem::weakly_canonical(child);

	// 2. 逐元素比较
	auto it_parent = norm_parent.begin();
	auto it_child = norm_child.begin();

	while (it_parent != norm_parent.end() && it_child != norm_child.end()) {
		if (*it_parent != *it_child) {
			return false; // 某个元素不匹配
		}
		++it_parent;
		++it_child;
	}
	// 父路径的所有元素都匹配，且父路径不比子路径长
	return it_parent == norm_parent.end();
}

bool ScanFile(const std::string& json_command_file, const std::string& scan_file_h,const std::string& out_put_json) {
	//检查文件是否存在
	if (!std::filesystem::exists(json_command_file)) {
		std::cerr << "json_command_file not exist" << std::endl;
	}
	if (!std::filesystem::exists(scan_file_h)) {
		std::cerr << "scan_file_h not exist" << std::endl;
	}

	std::filesystem::path scan_file_path(scan_file_h);
	//先扫描文件,获取文件所在路径
	auto base_path =scan_file_path.parent_path();
	std::vector<std::string> content = read_line_include_h(scan_file_h);
	auto all_abs_path =GetAbsPath(base_path,content);
	for (auto& s : all_abs_path) {
		std::cout<<"scan file:"<<s.string()<<std::endl;
	}

	//需要去重
	std::set<std::string> all_abs_path_unique;
	for (auto& s : all_abs_path) {
		if (std::filesystem::exists(s)) {
			all_abs_path_unique.insert(s.string());
		}
	}
	//读取json文件，找到任意一个符合条件的命令组，生成一个新的json文件
	std::ifstream ifs(json_command_file);
	nlohmann::json json_db = nlohmann::json::parse(ifs);
	nlohmann::json findJson;
	bool isFind = false;
	for (size_t i = 0; i < json_db.size(); ++i) {
		std::string str = json_db[i]["file"];
		std::filesystem::path json_file_path(str);
		if (is_parent_of(base_path, json_file_path)) {
			findJson = json_db[i];
			isFind = true;
		}
	}
	if (isFind) {
		nlohmann::json out = nlohmann::json::array();
		for (auto& s : all_abs_path_unique) {
			findJson["file"] = s;
			findJson["output"] = "";
			out.push_back(findJson);
		}
	
		std::cout << all_abs_path_unique.size()<<std::endl;;
		std::cout<<out.dump(4)<<std::endl;
		
		std::ofstream ofsm(out_put_json);
		if (ofsm.is_open()) {
			ofsm <<std::setw(4) << out<<std::endl;
			ofsm.close();
			return true;
		}
		return false;
	}

	return false;
}