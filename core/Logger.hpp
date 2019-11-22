#pragma once
#include "core/AsyncQueue.hpp"
#include "core/Thread.hpp"
#include <fstream>

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

        static Logger&      instance(std::string log_dir = "fpp_log");

        void                setLogLevel(LogLevel log_level);
        void                setFFmpegLogLevel(LogLevel log_level);

        void                print(const Object* caller, std::string code_position, LogLevel log_level, const std::string message);
        void                staticPrint(const std::string caller_name, std::string code_position, LogLevel log_level, const std::string message);

    private:

        Logger(std::string log_dir);
        virtual ~Logger() override;

        Logger(Logger const&)               = delete;
        Logger(Logger const&&)              = delete;
        Logger& operator=(Logger const&)    = delete;

    private:

        virtual Code        run() override;
        virtual Code        init() override;
        virtual Code        onStop() override;
        void                flush();
        void                openFile(std::string log_dir);
        void                closeFile();
        std::string         genFileName() const;
        std::string         formatMessage(const std::string caller_name, std::string code_position, LogLevel log_level, const std::string message);
        bool                ignoreMessage(LogLevel message_log_level);
        static void         log_callback(void* ptr, int level, const char* fmt, va_list vl);
        static LogLevel     convert_log_level(int ffmpeg_level);
        std::string         encodeLogLevel(LogLevel value);

    private:

        LogLevel            _log_level;
        MessageQueue        _message_queue;

        std::ofstream       _file;

    };

} // namespace fpp
