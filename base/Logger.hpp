#pragma once
#include "AsyncQueue.hpp"
#include "Thread.hpp"

namespace fpp {

    using Message = std::pair<LogLevel,std::string>;
    using MessageQueue = AsyncQueue<Message>;

    class Logger : public Thread {

    public:

        static Logger&     instance();
        void                setLogLevel(LogLevel log_level);
        void                setFfmpegLogLevel(LogLevel log_level);
        void                print(const Object* caller, std::string code_position, LogLevel log_level, std::string message);

    private:

        Logger();
        virtual ~Logger() override;

        Logger(Logger const&)               = delete;
        Logger(Logger const&&)              = delete;
        Logger& operator=(Logger const&)    = delete;

    private:

        Code                run() override;
        bool                ignoreMessage(LogLevel message_log_level);

    private:

        MessageQueue*       _messages;
        LogLevel            _log_level;

    };

} // namespace fpp
