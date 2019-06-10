#include "YThread.h"

#include "utils.h"

YThread::YThread() :
    YThread(std::bind(&YThread::run,this))
{
    //
}

YThread::YThread(std::function<YCode(void)> loop_function) :
    _running(false),
    _loop_function(loop_function)
{
    //
}

YThread &YThread::operator=(YThread&& other)
{
    _thread         = std::move(other._thread);
    _running        = other._running;
    _loop_function  = other._loop_function;
    return *this;
}

YThread::~YThread()
{
    quit();
}

void YThread::start()
{
    if (_running) { return; }
    _running = true;
    _thread = std::thread([this]() {
        while (_running && !utils::exit_code(_loop_function())) {}
        _running = false;
    });
    _thread.detach();
}

void YThread::quit()
{
    if (_running == false) { return; }
    _running = false;
    if (_thread.joinable()) { _thread.join(); }
}

bool YThread::running()
{
    return _running;
}

YCode YThread::run()
{
    return YCode::NOT_INITED;
}
