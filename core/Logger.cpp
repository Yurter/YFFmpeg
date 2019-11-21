#include "Logger.hpp"
#include "utils.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <Windows.h>
#include <ctime>
#include <filesystem>

namespace fpp {

    Logger::Logger() :
        _log_level(LogLevel::Info)
      , _log_dir("fpp_log")
    {
        setName("Logger");
//        av_log_set_callback(log_callback); //TODO later
//        set_ffmpeg_log_level(LogLevel::Error);
        print(this, code_pos, LogLevel::Info, "Logger opened.");
        openFile();
        try_throw(start());
    }

    Logger::~Logger() {
        print(this, code_pos, LogLevel::Info, "Logger closed.");
        av_log_set_callback(nullptr);
        flush();
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

        _file << message.log_text << std::endl;
        std::cout << message.log_text << std::endl;
        SetConsoleTextAttribute(hStdout, FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE);
        return Code::OK;
    }

    Code Logger::init() {
        setInited(true);
        return Code::OK;
    }

    void Logger::flush() {
        /* Прекращение получения новых сообщений */
        setLogLevel(LogLevel::Quiet);
        while (!_message_queue.empty()) {
            try_throw(run());
        }
    }

    void Logger::openFile() {
        std::filesystem::create_directory(_log_dir);
        _file.open(_log_dir + "/" + genFileName());
    }

    void Logger::closeFile() {
        _file.close();
    }

    std::string Logger::genFileName() const {
        time_t rawtime;
        struct tm* timeinfo;
        char buffer[80];

        time (&rawtime);
        timeinfo = localtime(&rawtime);

        strftime(buffer, 80, "%Y-%m-%d_%H-%M-%S.txt", timeinfo);
        return std::string(buffer);
    }

    std::string Logger::formatMessage(const std::string caller_name, std::string code_position, LogLevel log_level, const std::string message) {
        std::string header;

        header += "[" + encodeLogLevel(log_level) + "]";
        auto t = std::time(nullptr);
        header += "[" + (std::stringstream() << std::put_time(std::localtime(&t), "%H:%M:%S")).str() + "]";
        header += "[" + caller_name + "]";

        int message_offset = 27;

        if (log_level >= LogLevel::Debug) {
            header += "\n";
            header += (std::stringstream() << TAB << std::setw(message_offset) << std::left << "Thread id: " << current_thread_id()).str();
            header += "\n";
            header += (std::stringstream() << TAB << std::setw(message_offset) << std::left << "Code position: " << code_position).str();
            header += "\n";
            header += (std::stringstream() << TAB << std::setw(message_offset) << std::left << "Message: ").str();
        }

        std::stringstream ss;
        ss << std::setw(message_offset + 2) << std::left << header << message;

        return ss.str();
    }

    bool Logger::ignoreMessage(LogLevel message_log_level) {
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

        static_print_auto("FFmpeg", convert_log_level(level), line);
    }

    LogLevel Logger::convert_log_level(int ffmpeg_level) {
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

    std::string Logger::encodeLogLevel(LogLevel value) {
        switch (value) {
        case LogLevel::Info:
            return "I";
        case LogLevel::Warning:
            return "W";
        case LogLevel::Error:
            return "E";
        case LogLevel::Debug:
            return "D";
        case LogLevel::Trace:
            return "T";
        case LogLevel::Quiet:
            return "Q";
        }
        return "?";
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
        if (ignoreMessage(log_level)) { return; }
        std::string formated_message = formatMessage(caller->name(), code_position, log_level, message);
        if (!_message_queue.push(LogMessage(log_level, formated_message))) {
            // do nothing
        }
    }

    void Logger::staticPrint(const std::string caller_name, std::string code_position, LogLevel log_level, const std::string message) {
        if (ignoreMessage(log_level)) { return; }
        std::string formated_message = formatMessage(caller_name, code_position, log_level, message);
        if (!_message_queue.push(LogMessage(log_level, formated_message))) {
            // do nothing
        }
    }

} // namespace fpp
