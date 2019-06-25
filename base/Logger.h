#pragma once

#include "YAsyncQueue.h"
#include "YThread.h"

typedef std::pair<YLogLevel,std::string> YMessage;

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
    YAsyncQueue<YMessage>   _messages;
    YLogLevel               _log_level;

};
