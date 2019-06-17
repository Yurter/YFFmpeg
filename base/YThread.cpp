#include "YThread.h"
#include "utils.h"

#include <iostream>

YThread::YThread() :
    YThread(std::bind(&YThread::run,this))
{
    //
}

YThread::YThread(std::function<YCode(void)> loop_function) :
    _running(false),
    _loop_function(loop_function)
{
    setName("YThread");
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
        YCode ret = YCode::OK;
        while (_running && !utils::exit_code(ret = _loop_function())) {}
        _running = false;
        log_debug("Thread finished with code: " << ret
                  << " - " << utils::code_to_string(ret));
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

void YThread::join()
{
    while (running()) {
        utils::sleep_for(MEDIUM_DELAY_MS);
    }
}

YCode YThread::run()
{
    return YCode::NOT_INITED;
}
