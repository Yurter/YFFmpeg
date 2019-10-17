#pragma once
#include "AsyncQueue.hpp"
#include "Thread.hpp"

namespace fpp {

    using Message = std::pair<LogLevel,std::string>;
//    using MessageQueue = AsyncQueue<Message>;

    class Logger : public Thread
            , public AsyncQueue<Message> {

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

//        MessageQueue*       _messages;
        LogLevel            _log_level;

    };

} // namespace fpp
