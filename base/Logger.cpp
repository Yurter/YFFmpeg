#include "Logger.h"
#include "utils.h"
#include <iostream>
#include <string>
#include <sstream>
#include <windows.h>

Logger::Logger() :
    _log_level(YLogLevel::Warning)
{
    start();
}

Logger::~Logger()
{
    //
}

YCode Logger::run()
{
    YMessage message;
    if (!_messages.pop(message)) {
        utils::sleep_for(SHORT_DELAY_MS);
        return YCode::AGAIN;
    }

    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

    switch (message.first) {
    case YLogLevel::Quiet:
        break;
    case YLogLevel::Info:
        SetConsoleTextAttribute(hStdout, FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE);
        break;
    case YLogLevel::Warning:
        SetConsoleTextAttribute(hStdout, FOREGROUND_GREEN|FOREGROUND_RED|FOREGROUND_INTENSITY);
        break;
    case YLogLevel::Error:
        SetConsoleTextAttribute(hStdout, FOREGROUND_RED|FOREGROUND_INTENSITY);
        break;
    case YLogLevel::Debug:
        SetConsoleTextAttribute(hStdout, FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_INTENSITY);
        break;
    }

    std::cout << message.second << std::endl;
    SetConsoleTextAttribute(hStdout, FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE);
    return YCode::OK;
}

bool Logger::ignoreMessage(YLogLevel message_log_level)
{
    return message_log_level > _log_level;
}

Logger &Logger::instance()
{
    static Logger _logger;
    return _logger;
}

void Logger::setLogLevel(YLogLevel log_level)
{
    _log_level = log_level;
    switch (log_level) {
    case YLogLevel::Quiet:
        av_log_set_level(AV_LOG_QUIET);
        break;
    case YLogLevel::Info:
        av_log_set_level(AV_LOG_INFO);
        break;
    case YLogLevel::Warning:
        av_log_set_level(AV_LOG_WARNING);
        break;
    case YLogLevel::Error:
        av_log_set_level(AV_LOG_ERROR);
        break;
    case YLogLevel::Debug:
        av_log_set_level(AV_LOG_DEBUG);
        break;
    }
}

void Logger::print(YObject* caller, YLogLevel log_level, std::string message)
{
    if (ignoreMessage(log_level)) { return; }

    std::stringstream ss;

    if (log_level >= YLogLevel::Quiet) {
        //
    }
    if (log_level >= YLogLevel::Info) {
        ss << "[" << caller->name() << "]";
    }
    if (log_level >= YLogLevel::Warning) {
        //
    }
    if (log_level >= YLogLevel::Error) {
        //
    }
    if (log_level >= YLogLevel::Debug) {
        ss << "[" << current_thread_id() << "]";
    }

    ss << " " << message;
    _messages.push({log_level, ss.str()});
}

