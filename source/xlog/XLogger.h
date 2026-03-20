#include "xlogApi.h"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/base_sink.h>
#include <xsignal/XSignal.h>

extern XLOG_API std::shared_ptr<spdlog::logger>& xlogger; 
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

#define XLOG_TRACE( ...) SPDLOG_LOGGER_TRACE(xlogger, __VA_ARGS__)
#define XLOG_DEBUG( ...) SPDLOG_LOGGER_DEBUG(xlogger, __VA_ARGS__)
#define XLOG_INFO( ...) SPDLOG_LOGGER_INFO(xlogger, __VA_ARGS__)
#define XLOG_WARN( ...) SPDLOG_LOGGER_WARN(xlogger, __VA_ARGS__)
#define XLOG_ERROR(...) SPDLOG_LOGGER_ERROR(xlogger, __VA_ARGS__)
#define XLOG_CRITICAL( ...) SPDLOG_LOGGER_CRITICAL(xlogger, __VA_ARGS__)

class XLOG_API XForwardSink : public spdlog::sinks::base_sink<std::mutex> {
protected:
	 void sink_it_(const spdlog::details::log_msg& msg) override;
	 void flush_() override;
public:
	XSIGNAL(void(const std::string&,int level)) SigLog;
};

extern XLOG_API std::shared_ptr<XForwardSink> getXForwardSink();