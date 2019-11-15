#include "Logger.hpp"
#include "utils.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <Windows.h>

namespace fpp {

    Logger::Logger() :
        _log_level(LogLevel::Info)
    {
        setName("Logger");
//        av_log_set_callback(log_callback); //TODO later
//        set_ffmpeg_log_level(LogLevel::Error);
        try_throw(start());
    }

    Logger::~Logger() {
        print_info("Closed.");
        flush();
        av_log_set_callback(nullptr);
    }

    Code Logger::run() {
        LogMessage message;
        return_if_not(_message_queue.wait_and_pop(message), Code::EXIT);

        HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

        switch (message.log_level) {
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
        case LogLevel::Trace:
            SetConsoleTextAttribute(hStdout, FOREGROUND_BLUE|FOREGROUND_INTENSITY|FOREGROUND_RED);
            break;
        }

        std::cout << message.log_text << std::endl;
        SetConsoleTextAttribute(hStdout, FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE);
        return Code::OK;
    }

    void Logger::flush() {
        /* Прекращение получения новых сообщений */
        setLogLevel(LogLevel::Quiet);
        while (!_message_queue.empty()) {
            try_throw(run());
        }
    }

    std::string Logger::format_message(const std::string caller_name, std::string code_position, LogLevel log_level, const std::string message) {
        std::string header = "[" + caller_name + "]";

        if (log_level >= LogLevel::Debug) {
            header += "\n";
            header += (std::stringstream() << TAB << std::setw(20) << std::left << "Thread id: " << current_thread_id()).str();
            header += "\n";
            header += (std::stringstream() << TAB << std::setw(20) << std::left << "Code position: " << code_position).str();
            header += "\n";
        }

        if (log_level >= LogLevel::Debug) {
            header += (std::stringstream() << TAB << std::setw(20) << std::left << "Message: " /*<< message*/).str();
        }

        std::stringstream ss;
        ss << std::setw(22) << std::left << header << message;

        return ss.str();
    }

    bool Logger::ignore_message(LogLevel message_log_level) {
        return message_log_level > _log_level;
    }

    void Logger::log_callback(void* ptr, int level, const char* fmt, va_list vl) {
        va_list vl2;
        char line[1024];
        static int print_prefix = 1;

        va_copy(vl2, vl);
        av_log_default_callback(ptr, level, fmt, vl);
        av_log_format_line(ptr, level, fmt, vl2, line, sizeof(line), &print_prefix);
        va_end(vl2);

        static_print_auto("FFmpeg", convertLogLevel(level), line);
    }

    LogLevel Logger::convertLogLevel(int ffmpeg_level) {
        switch (ffmpeg_level) {
        case AV_LOG_QUIET:
            return LogLevel::Quiet;
        case AV_LOG_PANIC:
        case AV_LOG_FATAL:
        case AV_LOG_ERROR:
            return LogLevel::Error;
        case AV_LOG_WARNING:
            return LogLevel::Warning;
        case AV_LOG_INFO:
            return LogLevel::Info;
        default:
            return LogLevel::Quiet;
        }
    }

    Logger& Logger::instance() {
        static Logger _logger;
        return _logger;
    }

    void Logger::setLogLevel(LogLevel log_level) {
        _log_level = log_level;
    }

    void Logger::setFFmpegLogLevel(LogLevel log_level) {
        switch (log_level) {
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
        case LogLevel::Trace:
            av_log_set_level(AV_LOG_TRACE);
            break;
        case LogLevel::Quiet:
            av_log_set_level(AV_LOG_QUIET);
            break;
        }
    }

    void Logger::print(const Object* caller, std::string code_position, LogLevel log_level, const std::string message) {
        if (ignore_message(log_level)) { return; }
        std::string formated_message = format_message(caller->name(), code_position, log_level, message);
        if (!_message_queue.push(LogMessage(log_level, formated_message))) {
            // do nothing
        }
//        if (!wait_and_push(Message(log_level, formated_message))) {
//            // do nothing
//        }
    }

    void Logger::static_print(const std::string caller_name, std::string code_position, LogLevel log_level, const std::string message) {
        if (ignore_message(log_level)) { return; }
        std::string formated_message = format_message(caller_name, code_position, log_level, message);
        if (!_message_queue.push(LogMessage(log_level, formated_message))) {
            // do nothing
        }
//        if (!wait_and_push(Message(log_level, formated_message))) {
//            // do nothing
//        }
    }

} // namespace fpp
