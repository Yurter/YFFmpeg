#pragma once

#include "ffmpeg.h"
#include "YAsyncQueue.h"
#include "YThread.h"
#include <string>

#define set_log_level(x)    Logger::instance().setLogLevel(x)
#define log_info(x)         Logger::instance().print(this, YLogLevel::Info, x)
#define log_warning(x)      Logger::instance().print(this, YLogLevel::Warning, x)
#define log_error(x)        Logger::instance().print(this, YLogLevel::Error, x)
#define log_debug(x)        Logger::instance().print(this, YLogLevel::Debug, x)
//#define log_debug(x)        Logger::instance().print(this, YLogLevel::Debug, std::string(__FUNCTION__) + x)

class Logger : public YThread
{

public:

    static Logger& instance();

    void setLogLevel(YLogLevel log_level);
    void print(YObject* caller, YLogLevel log_level, std::string message);

private:

    Logger();
    ~Logger();

    Logger(Logger const&)               = delete;
    Logger& operator=(Logger const&)    = delete;

    YCode run();

    bool ignoreMessage(YLogLevel message_log_level);

private:

    // General
    YAsyncQueue<std::pair<YLogLevel,std::string>>    _messages;
    YLogLevel                   _log_level;

};
