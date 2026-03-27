#include "XLogger.h"
#include "spdlog/sinks/stdout_color_sinks.h"					// 控制台彩色输出
#include "spdlog/sinks/basic_file_sink.h"						// 基础文件输出（单文件）

static std::string console_wapPattern = "[%Y-%m-%d %H:%M:%S.%e] [%t] [%n] [%^%l%$] (%s %#)  %v";		//消息放到最后面，主要用于换行的消息
static std::string console_pattern = "[%Y-%m-%d %H:%M:%S.%e] [%t] [%n] [%^%l%$] %v";
static std::string forward_pattern = "[%Y-%m-%d %H:%M:%S.%e] [%t] [%n] [%^%l%$] %v";

std::shared_ptr<spdlog::logger>& getLogger() {
	static std::shared_ptr<spdlog::logger> logger;
	if (logger) {
		return logger;
	}
	
	/*
	[% Y - % m - % d % H:% M : % S. % e] → 时间
		[% t]                     → 线程 ID
		[% ^%l % $]                 → 带颜色的日志级别（仅控制台有效）
		% v                       → 用户日志消息（即 info("...") 中的内容）
		(%s % #)                  → 源文件名 + 行号
	*/

	// 1. 写入到控制台的sink
	auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
	console_sink->set_pattern(console_pattern);

	// 2. 写入到文件的sink
	auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/xlogger.log", true);
	file_sink->set_pattern(console_pattern);

	// 3. 自定义转发sink
	auto forward_sink = std::make_shared<XForwardSink>();
	forward_sink->set_pattern(forward_pattern);

	spdlog::logger multi_logger("xlogger", { console_sink, forward_sink,file_sink });

	 logger = std::make_shared<spdlog::logger>(multi_logger);

	logger->set_level(spdlog::level::trace);

	SPDLOG_LOGGER_TRACE(logger, "XLogger init success");

	return logger;
}

std::shared_ptr<spdlog::logger>& xlogger = getLogger();

void XForwardSink::sink_it_(const spdlog::details::log_msg& msg)
{
	// 格式化消息
	spdlog::memory_buf_t formatted;
	spdlog::sinks::base_sink<std::mutex>::formatter_->format(msg, formatted);
	auto str =fmt::to_string(formatted);
	SigLog(str,msg.level);
}

void XForwardSink::flush_()
{
	//do nth
}

 std::shared_ptr<XForwardSink> getXForwardSink()
{
	auto logger =getLogger();
	for (auto sink : logger->sinks()) {
		if (std::dynamic_pointer_cast<XForwardSink>(sink)) {
			return std::dynamic_pointer_cast<XForwardSink>(sink);
		}
	}
	return nullptr;
}

  void setXlogPatternLogEnd()
 {
	 auto logger = getLogger();
	 for (auto sink : logger->sinks()) {
		 if (auto console_sink = std::dynamic_pointer_cast<spdlog::sinks::stdout_color_sink_mt>(sink)) {
			 console_sink->set_pattern(console_wapPattern);
		 }
		 if (auto file_sink = std::dynamic_pointer_cast<spdlog::sinks::basic_file_sink_mt>(sink)) {
			 file_sink->set_pattern(console_wapPattern);
		 }

	 }
	
 }

  void setXlogPatternLogDefault()
 {
	 auto logger = getLogger();
	 for (auto sink : logger->sinks()) {
		 if (auto console_sink = std::dynamic_pointer_cast<spdlog::sinks::stdout_color_sink_mt>(sink)) {
			 console_sink->set_pattern(console_pattern);
		 }
		 if (auto file_sink = std::dynamic_pointer_cast<spdlog::sinks::basic_file_sink_mt>(sink)) {
			 file_sink->set_pattern(console_pattern);
		 }
	 }
 }
