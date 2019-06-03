#include "YThread.h"

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
    _running = true;
    _thread = std::thread([this]() {
        while (_running) { run(); }
    });
    _thread.detach();
}

void YThread::quit()
{
    if (_running == false) { return; }
    _running = false;
    if (_thread.joinable()) { _thread.join(); }
}
