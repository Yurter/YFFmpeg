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

    Logger::Logger(std::string log_dir) :
        _log_level(LogLevel::Info)
    {
        setName("Logger");
//        av_log_set_callback(log_callback); //TODO later
        setFFmpegLogLevel(LogLevel::Error);
        print(this->name(), code_pos, LogLevel::Info, "Logger opened.");
        openFile(log_dir);
    }

    Logger::~Logger() {
        print(this->name(), code_pos, LogLevel::Info, "Logger closed.");
        av_log_set_callback(nullptr);
        closeFile();
    }

    Code Logger::print(const LogMessage& message) {
        std::lock_guard lock(_print_mutex);
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

    void Logger::openFile(const std::string &log_dir) {
        std::filesystem::create_directory(log_dir);
        _file.open(log_dir + "/" + genFileName());
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

    std::string Logger::formatMessage(std::string caller_name, const std::string &code_position, LogLevel log_level, const std::string& message) {
        std::string header;

        const size_t max_name_length = 20;
        const int message_offset = 30;

        caller_name.resize(max_name_length, ' ');

        header += "[" + encodeLogLevel(log_level) + "]";
        const int64_t t = std::time(nullptr);
        header += "[" + (std::stringstream() << std::put_time(std::localtime(&t), "%H:%M:%S")).str() + "]"; //TODO if Debug выводить мс через хроно, вынести формирование временной метки в метод
        header += "[" + caller_name + "]";

        if (log_level >= LogLevel::Debug) {
            std::string thread_id = (std::stringstream() << "[" << current_thread_id() << "]").str();
            thread_id.resize(7, ' ');
            header += " " + thread_id;
        }
        if (log_level >= LogLevel::Trace) {
            header += "\n";
            header += (std::stringstream() << TAB << std::setw(message_offset) << std::left << "Code position: " << code_position).str();
            header += "\n";
            header += (std::stringstream() << TAB << std::setw(message_offset) << std::left << "Message: ").str();
        }

        std::stringstream ss;
        ss << header << " " << message;

        return ss.str();
    }

    void Logger::log_callback(void* ptr, int level, const char* fmt, va_list vl) {
        va_list vl2;
        char line[1024];
        static int print_prefix = 1;

        va_copy(vl2, vl);
        av_log_default_callback(ptr, level, fmt, vl);
        av_log_format_line(ptr, level, fmt, vl2, line, sizeof(line), &print_prefix);
        va_end(vl2);

        for (auto& symbol : line) {
            if (symbol == '\n') {
                symbol = ' ';
                break;
            }
        }

        static_log_auto("FFmpeg", convert_log_level(level), line);
    }

    LogLevel Logger::convert_log_level(int ffmpeg_level) { //TODO
//        switch (ffmpeg_level) {
//        case AV_LOG_QUIET:
//            return LogLevel::Quiet;
//        case AV_LOG_PANIC:
//        case AV_LOG_FATAL:
//        case AV_LOG_ERROR:
//            return LogLevel::Error;
//        case AV_LOG_WARNING:
//            return LogLevel::Warning;
//        case AV_LOG_INFO:
//            return LogLevel::Info;
//        default:
//            return LogLevel::Quiet;
//        }
        if (ffmpeg_level < AV_LOG_INFO) {
            return LogLevel::Error;
        } else {
            return LogLevel::Quiet;
        }
    }

    std::string Logger::encodeLogLevel(LogLevel value) {
//        switch (value) {
//        case LogLevel::Info:
//            return "I";
//        case LogLevel::Warning:
//            return "W";
//        case LogLevel::Error:
//            return "E";
//        case LogLevel::Debug:
//            return "D";
//        case LogLevel::Trace:
//            return "T";
//        case LogLevel::Quiet:
//            return "Q";
//        }
        switch (value) {
        case LogLevel::Info:
            return "info";
        case LogLevel::Warning:
            return "warn";
        case LogLevel::Error:
            return "err ";
        case LogLevel::Debug:
            return "deb ";
        case LogLevel::Trace:
            return "trac";
        case LogLevel::Quiet:
            return "   ";
        }
        return "?";
    }

    Logger& Logger::instance(std::string log_dir) {
        static Logger _logger(log_dir);
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

    bool Logger::ignoreMessage(const LogLevel message_log_level) {
        return (message_log_level == LogLevel::Quiet) || (message_log_level > _log_level);
    }

    void Logger::print(const std::string& caller_name, const std::string& code_position, const LogLevel log_level, const std::string& message) {
        const std::string formated_message = formatMessage(caller_name, code_position, log_level, message);
        try_throw(print({ log_level, formated_message }));
    }

} // namespace fpp
