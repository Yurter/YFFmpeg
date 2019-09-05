#include "YLogger.hpp"
#include "utils.hpp"
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <Windows.h>

YLogger::YLogger() :
    _messages(new MessageQueue),
    _log_level(YLogLevel::Info)
{
    setName("YLogger");
    start();
}

YLogger::~YLogger() {
    _messages->clear();
    delete _messages;
}

YCode YLogger::run() {
    Message message;
    guaranteed_pop(_messages, message);

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

bool YLogger::ignoreMessage(YLogLevel message_log_level) {
    return message_log_level > _log_level;
}

YLogger& YLogger::instance() {
    static YLogger _logger;
    return _logger;
}

void YLogger::setLogLevel(YLogLevel log_level) {
    _log_level = log_level;
}

void YLogger::setFfmpegLogLevel(YLogLevel log_level) {
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

void YLogger::print(const YObject* caller, std::string code_position, YLogLevel log_level, std::string message) {
    if (ignoreMessage(log_level)) { return; }

    std::string header;

    if (log_level > YLogLevel::Quiet) {
        header += "[" + caller->name() + "]";
    }
    if (log_level >= YLogLevel::Debug) {
        header += "\n";
        header += (std::stringstream() << TAB << std::setw(20) << std::left << "Thread id: " << current_thread_id()).str();
        header += "\n";
        header += (std::stringstream() << TAB << std::setw(20) << std::left << "Code position: " << code_position).str();
        header += "\n";
    }

    if (log_level >= YLogLevel::Debug) {
        message = (std::stringstream() << TAB << std::setw(20) << std::left << "Message: " << message).str();
    }

    std::stringstream ss;

    ss << std::setw(22) << std::left << header << message;

    guaranteed_push(_messages, Message(log_level, ss.str()));
}
