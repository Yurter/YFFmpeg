#include "YThread.h"

#include "utils.h"

YThread::YThread() :
    _running(false)
{
    //
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
//        while (_running) { run(); }
        while (_running && !utils::exit_code(run())) {}
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
