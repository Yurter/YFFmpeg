#pragma once

#include "YAsyncQueue.hpp"
#include "YThread.hpp"

using Message = std::pair<YLogLevel,std::string>;
using MessageQueue = YAsyncQueue<Message>;

class YLogger : public YThread
{

public:

    static YLogger&     instance();
    void                setLogLevel(YLogLevel log_level);
    void                setFfmpegLogLevel(YLogLevel log_level);
    void                print(const YObject* caller, std::string code_position, YLogLevel log_level, std::string message);

private:

    YLogger();
    virtual ~YLogger() override;

    YLogger(YLogger const&)               = delete;
    YLogger(YLogger const&&)              = delete;
    YLogger& operator=(YLogger const&)    = delete;

private:

    YCode               run() override;
    bool                ignoreMessage(YLogLevel message_log_level);

private:

    MessageQueue*       _messages;
    YLogLevel           _log_level;

};
