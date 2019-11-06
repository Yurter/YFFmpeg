#pragma once
#include "core/AsyncQueue.hpp"
#include "core/Thread.hpp"

namespace fpp {

//    using Message = std::pair<LogLevel,std::string>;
    class LogMessage {

    public:

        LogMessage() {}
        LogMessage(LogLevel level, std::string text) :
            log_level(level)
            , log_text(text) {}

        uint64_t    size() const { return 1; /*log_text.size();*/ }

        LogLevel    log_level;
        std::string log_text;

    };

    using MessageQueue = AsyncQueue<LogMessage>;

    class Logger : public Thread {

    public:

        static Logger&      instance();
        void                setLogLevel(LogLevel log_level);
        void                setFfmpegLogLevel(LogLevel log_level);
        void                print(const Object* caller, std::string code_position, LogLevel log_level, const std::string message);
        void                static_print(const std::string caller_name, std::string code_position, LogLevel log_level, const std::string message);

    private:

        Logger();
        virtual ~Logger() override;

        Logger(Logger const&)               = delete;
        Logger(Logger const&&)              = delete;
        Logger& operator=(Logger const&)    = delete;

    private:

        virtual Code        run() override;
        void                flush();
        std::string         format_message(const std::string caller_name, std::string code_position, LogLevel log_level, const std::string message);
        bool                ignore_message(LogLevel message_log_level);
        static void         log_callback(void* ptr, int level, const char* fmt, va_list vl);
        static LogLevel     convertLogLevel(int ffmpeg_level);

    private:

        LogLevel            _log_level;
        MessageQueue        _message_queue;

    };

} // namespace fpp
