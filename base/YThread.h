#pragma once

#include "ffmpeg.h"
#include "YObject.h"
#include <thread>

class YThread : public YObject
{

public:

    YThread();
    YThread(std::function<YCode(void)> loop_function);
    YThread& operator=(YThread&& other);
    YThread(const YThread&)             = delete;
    YThread& operator=(const YThread&)  = delete;
    virtual ~YThread();

    virtual void        start()     final;
    virtual void        quit()      final;
    virtual bool        running()   final;
    virtual void        join()      final;

protected:

    virtual YCode       run();

private:

    // General
    std::thread                 _thread;
    volatile bool               _running;
    std::function<YCode(void)>  _loop_function;


};

