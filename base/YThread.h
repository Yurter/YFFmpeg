#pragma once

#include "ffmpeg.h"
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

    virtual void        start()     final;
    virtual void        quit()      final;
    virtual bool        running()   final;
    virtual void        join()      final;

    void                setExitCode(YCode exit_code);
    YCode               exitCode() const;

protected:

    virtual YCode       run();

private:

    // General
    std::thread         _thread;
    volatile bool       _running;
    YCode               _exit_code;
    LoopFunction        _loop_function;

};

