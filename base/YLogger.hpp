#pragma once

#include "YAsyncQueue.hpp"
#include "YThread.hpp"

typedef std::pair<YLogLevel,std::string>    Message;
typedef YAsyncQueue<Message>                MessageQueue;

class YLogger : public YThread
{

public:

    static YLogger&     instance();
    void                setLogLevel(YLogLevel log_level);
    void                setFfmpegLogLevel(YLogLevel log_level);
    void                print(const YObject* caller, YLogLevel log_level, std::string message);

private:

    YLogger();
    virtual ~YLogger() override;

    YLogger(YLogger const&)               = delete;
    YLogger& operator=(YLogger const&)    = delete;

private:

    YCode               run() override;
    bool                ignoreMessage(YLogLevel message_log_level);

private:

    MessageQueue*       _messages;
    YLogLevel           _log_level;

};
