#pragma once
#include "Object.hpp"
#include <fstream>
#include <mutex>

namespace fpp {

    class Logger : public Object {

    public:

        static Logger&      instance(std::string log_dir = "fpp_log");

        void                setLogLevel(LogLevel log_level);
        void                setFFmpegLogLevel(LogLevel log_level);

        bool                ignoreMessage(const LogLevel message_log_level);
        void                print(const std::string& caller_name, const std::string& code_position, const LogLevel log_level, const std::string& message);

    private:

        Logger(std::string log_dir);
        virtual ~Logger() override;

        Logger(Logger const&)               = delete;
        Logger(Logger const&&)              = delete;
        Logger& operator=(Logger const&)    = delete;

    private:

        Code                print(const LogLevel log_level, const std::string& log_text);
        void                openFile(const std::string& log_dir);
        void                closeFile();
        std::string         genFileName() const;
        std::string         formatMessage(std::string caller_name, const std::string& code_position, LogLevel log_level, const std::string& message);
        std::string         getTimeStamp(LogLevel log_level) const;
        static void         log_callback(void* ptr, int level, const char* fmt, va_list vl);
        static LogLevel     convert_log_level(int ffmpeg_level);
        std::string         encodeLogLevel(LogLevel value);
        std::string         shortenCodePosition(const std::string& value) const;

    private:

        LogLevel            _log_level;
        std::ofstream       _file;
        std::mutex          _print_mutex;

    };

} // namespace fpp
