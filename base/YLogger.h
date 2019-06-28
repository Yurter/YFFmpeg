#pragma once

#include "YAsyncQueue.h"
#include "YThread.h"

typedef std::pair<YLogLevel,std::string>    YMessage;
typedef YAsyncQueue<YMessage>               MessageQueue;

class YLogger : public YThread
{

public:

    static YLogger&     instance();
    void                setLogLevel(YLogLevel log_level);
    void                print(const YObject* caller, YLogLevel log_level, std::string message);

    std::string         toString() const final;

private:

    YLogger();
    virtual ~YLogger() override = default;

    YLogger(YLogger const&)               = delete;
    YLogger& operator=(YLogger const&)    = delete;

    YCode               run() override;
    bool                ignoreMessage(YLogLevel message_log_level);

private:

    // General
    MessageQueue        _messages;
    YLogLevel           _log_level;

};
