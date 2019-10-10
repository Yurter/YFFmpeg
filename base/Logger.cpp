#include "Logger.hpp"
#include "utils.hpp"
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <Windows.h>

namespace fpp {

    Logger::Logger() :
        _messages(new MessageQueue),
        _log_level(LogLevel::Info)
    {
        setName("Logger");
        start();
    }

    Logger::~Logger() {
        _messages->clear();
        delete _messages;
    }

    Code Logger::run() {
        Message message;
        guaranteed_pop(_messages, message);

        HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

        switch (message.first) {
        case LogLevel::Quiet:
            break;
        case LogLevel::Info:
            SetConsoleTextAttribute(hStdout, FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE);
            break;
        case LogLevel::Warning:
            SetConsoleTextAttribute(hStdout, FOREGROUND_GREEN|FOREGROUND_RED|FOREGROUND_INTENSITY);
            break;
        case LogLevel::Error:
            SetConsoleTextAttribute(hStdout, FOREGROUND_RED|FOREGROUND_INTENSITY);
            break;
        case LogLevel::Debug:
            SetConsoleTextAttribute(hStdout, FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_INTENSITY);
            break;
        }

        std::cout << message.second << std::endl;
        SetConsoleTextAttribute(hStdout, FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE);
        return Code::OK;
    }

    bool Logger::ignoreMessage(LogLevel message_log_level) {
        return message_log_level > _log_level;
    }

    Logger& Logger::instance() {
        static Logger _logger;
        return _logger;
    }

    void Logger::setLogLevel(LogLevel log_level) {
        _log_level = log_level;
    }

    void Logger::setFfmpegLogLevel(LogLevel log_level) {
        switch (log_level) {
        case LogLevel::Quiet:
            av_log_set_level(AV_LOG_QUIET);
            break;
        case LogLevel::Info:
            av_log_set_level(AV_LOG_INFO);
            break;
        case LogLevel::Warning:
            av_log_set_level(AV_LOG_WARNING);
            break;
        case LogLevel::Error:
            av_log_set_level(AV_LOG_ERROR);
            break;
        case LogLevel::Debug:
            av_log_set_level(AV_LOG_DEBUG);
            break;
        }
    }

    void Logger::print(const Object* caller, std::string code_position, LogLevel log_level, std::string message) {
        if (ignoreMessage(log_level)) { return; }

        std::string header;

        if (log_level > LogLevel::Quiet) {
            header += "[" + caller->name() + "]";
        }
        if (log_level >= LogLevel::Debug) {
            header += "\n";
            header += (std::stringstream() << TAB << std::setw(20) << std::left << "Thread id: " << current_thread_id()).str();
            header += "\n";
            header += (std::stringstream() << TAB << std::setw(20) << std::left << "Code position: " << code_position).str();
            header += "\n";
        }

        if (log_level >= LogLevel::Debug) {
            message = (std::stringstream() << TAB << std::setw(20) << std::left << "Message: " << message).str();
        }

        std::stringstream ss;

        ss << std::setw(22) << std::left << header << message;

        guaranteed_push(_messages, Message(log_level, ss.str()));
    }

} // namespace fpp
