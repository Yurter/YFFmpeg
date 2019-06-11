#pragma once

#include "ffmpeg.h"
#include "YAsyncQueue.h"

#include <string>

#define info_log(x)     Logger::print(YLogLevel::Info, x)
#define warning_log(x)  Logger::print(YLogLevel::Warning, x)
#define error_log(x)    Logger::print(YLogLevel::Error, x)
#define debug_log(x)    Logger::print(YLogLevel::Debug, x)

class Logger
{

public:

    Logger(); //TODO singletone
    ~Logger();

    static void setLogLevel(YLogLevel log_level);
    static void print(YLogLevel log_level, std::string message);

private:

    // General
    YAsyncQueue<std::string>    _messages;
    YLogLevel                   _log_level;

};
