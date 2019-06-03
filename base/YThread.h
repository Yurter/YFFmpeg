#pragma once

#include "ffmpeg.h"
#include <thread>

class YThread
{

public:

    YThread();
    virtual ~YThread();

    virtual void        start();
    virtual void        quit()      final;
    virtual bool        running()   final;

protected:

    virtual YCode       run() = 0;

private:

    // General
    std::thread         _thread;
    volatile bool       _running;

};

