#pragma once

#include <thread>

class YThread
{

public:

    YThread();
    virtual ~YThread();

    virtual void        start() final;
    virtual void        quit()  final;

protected:

    virtual void        run() = 0;

private:

    // General
    std::thread         _thread;
    volatile bool       _running;

};

