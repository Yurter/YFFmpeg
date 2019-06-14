#pragma once

#include "ffmpeg.h"
#include "YAsyncQueue.h"
#include "YThread.h"
#include <string>
#include <sstream>

#define set_log_level(x)    Logger::instance().setLogLevel(x)

#define print_info(x)       Logger::instance().print(this, YLogLevel::Info, x)
#define print_warning(x)    Logger::instance().print(this, YLogLevel::Warning, x)
#define print_error(x)      Logger::instance().print(this, YLogLevel::Error, x)
#define print_debug(x)      Logger::instance().print(this, YLogLevel::Debug, x)

#define log_info(x)         { std::stringstream log_ss; log_ss << x; print_info(log_ss.str());      }
#define log_warning(x)      { std::stringstream log_ss; log_ss << x; print_warning(log_ss.str());   }
#define log_error(x)        { std::stringstream log_ss; log_ss << x; print_error(log_ss.str());     }
#define log_debug(x)        { std::stringstream log_ss; log_ss << x; print_debug(log_ss.str());     }

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
