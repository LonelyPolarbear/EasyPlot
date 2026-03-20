#include "XLogger.h"
#include "spdlog/sinks/stdout_color_sinks.h"					// 控制台彩色输出
#include "spdlog/sinks/basic_file_sink.h"						// 基础文件输出（单文件）
std::shared_ptr<spdlog::logger>& getLogger() {
	static std::shared_ptr<spdlog::logger> logger;
	if (logger) {
		return logger;
	}
	
	// 1. 写入到控制台的sink
	auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
	console_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%t] [%n] [%^%l%$] %v (%s %#)");

	// 2. 写入到文件的sink
	auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/xlogger.log", true);
	file_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%t] [%^%l%$] %v (%s %#)");

	// 3. 自定义转发sink
	auto forward_sink = std::make_shared<XForwardSink>();
	forward_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%t] [%^%l%$] %v");

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
