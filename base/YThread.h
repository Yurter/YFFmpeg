#pragma once

#include "ffmpeg.h"
#include <thread>

class YThread
{

public:

    YThread();
    YThread(std::function<YCode(void)>* loop_function);
    virtual ~YThread();

    virtual void        start()     final;
    virtual void        quit()      final;
    virtual bool        running()   final;

protected:

    virtual YCode       run();

private:

    // General
    std::thread                 _thread;
    volatile bool               _running;
    std::function<YCode(void)>* _loop_function;

};

