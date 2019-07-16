#pragma once

#include "YObject.h"
#include <thread>
#include <functional>

typedef std::function<YCode(void)> LoopFunction;

class YThread : public YObject
{

public:

    YThread();
    YThread(LoopFunction loop_function);
    YThread& operator=(YThread&& other);
    YThread(const YThread&)             = delete;
    YThread& operator=(const YThread&)  = delete;
    virtual ~YThread() override;

    virtual YCode       start()     final;
    virtual YCode       quit()      final;
    virtual bool        running()   final;
    virtual void        join()      final;

    YCode               exitCode()      const;
    std::string         exitMessage()   const;

protected:

    virtual YCode       run();

private:

    std::thread         _thread;
    volatile bool       _running;
    YCode               _exit_code;
    std::string         _exit_message;
    LoopFunction        _loop_function;

};

