#define Py_DEBUG 0
#define Py_REF_DEBUG 0
#define Py_TRACE_REFS 0

#include <iostream>
#include <dataBase/XVector.h>
#include <dataBase/XDataObject.h>
#include <dataBase/XDataArray.h>
#include <xalgo/XAlgo.h>
#include <lib00_utilty/XUtilty.h>
#include <numeric>

#include <xtest/XTest.h>
#include <xlog/XLogger.h>


#include <spdlog/spdlog.h>
#include "spdlog/sinks/stdout_color_sinks.h"		// 控制台彩色输出
#include "spdlog/sinks/basic_file_sink.h"			// 基本文件输出
#include "spdlog/sinks/rotating_file_sink.h"		// 滚动文件输出
#include "spdlog/async.h"										// 异步模式（可选）


#if 1
void console_log(spdlog::level::level_enum level) {
	auto console = spdlog::get("console");
	if (!console) {
		console = spdlog::stdout_color_mt("console");
	}
	// 设置全局日志级别
	console->set_level(level);
	// 设置日志格式（时间、日志级别、消息）
	// 设置日志格式：时间戳 [线程ID] [logger名称] [级别] 实际消息 (文件名:行号)
	// console->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");

	console->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%t] [%n] [%^%l%$] %v (%s %#)");
	/*
	%@			源文件名 + 行号(最常用)			main.cpp:42
	%s			源文件名（不含路径）				main.cpp
	%#			源文件行号								42
	%!			函数名称									main 或 my_function
	*/
	// 写日志
	console->trace("0 this is a trace message!");
	console->debug("1 This is a debug message with number {}", 42);
	console->info("2 info:Welcome to spdlog!");
	console->warn("3 Warning: something might be wrong");
	console->error("4 Error occurred: {}", errno);
	console->critical("5 Critical error: shutting down");

	SPDLOG_LOGGER_TRACE(console, "0 this is a trace message!");
}

void file_log(spdlog::level::level_enum level) {
	auto file_logger = spdlog::get("file_logger");
	if (!file_logger) {
		file_logger = spdlog::basic_logger_mt("file_logger", "basic.log");
		file_logger->flush_on(spdlog::level::trace); //设置一个触发刷新的日志级别阈值
		//file_logger->flush();  手动刷新
	}
	
	file_logger->set_level(level);
	file_logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%t] [%n] [%^%l%$] %v (%@)");

	// 写日志
	file_logger->trace("0 this is a trace message!");
	file_logger->debug("1 This is a debug message with number {}", 42);
	file_logger->info("2 info:Welcome to spdlog!");
	file_logger->warn("3 Warning: something might be wrong");
	file_logger->error("4 Error occurred: {}", errno);
	file_logger->critical("5 Critical error: shutting down");
}

void xlogger_test() {

	XLOG_TRACE("0 this is a trace message!");
	XLOG_DEBUG( "1 This is a debug message");
	XLOG_INFO( "2 info:Welcome to spdlog!");
	XLOG_WARN( "3 Warning: something might be wrong");
	XLOG_ERROR( "4 Error occurred: {}", errno);
	XLOG_CRITICAL( "5 Critical error: shutting down");

	XLOG_INFO(20);
}
/*
enum level_enum : int {
	trace = SPDLOG_LEVEL_TRACE,			//最详细的追踪信息，常用于深入调试程序执行流程，例如函数进出、变量值变化等。生产环境通常关闭。
	debug = SPDLOG_LEVEL_DEBUG,			//调试信息，帮助开发者定位问题，比 trace 更粗略，但仍主要用于开发阶段
	info = SPDLOG_LEVEL_INFO,					//一般信息，记录程序正常运行过程中的关键事件，如服务启动、配置加载完成等。
	warn = SPDLOG_LEVEL_WARN,				//警告，表示可能出现问题或存在不推荐的做法，但程序仍能继续运行，例如磁盘空间不足
	err = SPDLOG_LEVEL_ERROR,				//错误，发生了错误，但程序可能还能继续运行，例如无法打开配置文件但使用默认值
	critical = SPDLOG_LEVEL_CRITICAL,	//严重错误，通常导致程序无法继续正常运行，例如内存分配失败、数据库连接丢失等
	off = SPDLOG_LEVEL_OFF,						//关闭所有日志输出，不记录任何信息
	n_levels
};
*/

int main() {
	XTestApp app;
	app.setName("XlogTest")->setDesc("这是对spdlog库的使用演示");

	app.addCmd("console_log_trace", "trace", std::bind(console_log, spdlog::level::trace));
	app.addCmd("console_log_info", "info", std::bind(console_log, spdlog::level::info));
	app.addCmd("console_log_debug", "debug", std::bind(console_log, spdlog::level::debug));
	app.addCmd("console_log_warn", "warn", std::bind(console_log, spdlog::level::warn));
	app.addCmd("console_log_err", "err", std::bind(console_log, spdlog::level::err));
	app.addCmd("console_log_critical", "critical", std::bind(console_log, spdlog::level::critical));
	app.addCmd("console_log_off", "off", std::bind(console_log, spdlog::level::off));

	app.addCmd("file_log_trace", "trace", std::bind(file_log, spdlog::level::trace));
	app.addCmd("file_log_info", "info", std::bind(file_log, spdlog::level::info));
	app.addCmd("file_log_debug", "debug", std::bind(file_log, spdlog::level::debug));
	app.addCmd("file_log_warn", "warn", std::bind(file_log, spdlog::level::warn));
	app.addCmd("file_log_err", "err", std::bind(file_log, spdlog::level::err));
	app.addCmd("file_log_critical", "critical", std::bind(file_log, spdlog::level::critical));
	app.addCmd("file_log_off", "off", std::bind(file_log, spdlog::level::off));

	app.addCmd("xlogger_test", "xlogger_test", xlogger_test);

	return app.run();
}

#endif