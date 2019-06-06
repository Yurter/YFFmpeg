#include "YThread.h"

#include "utils.h"

YThread::YThread() :
    _running(false),
    _loop_function(nullptr)
{
    //
}

YThread::YThread(std::function<YCode(void)>* loop_function) :
    _loop_function(loop_function)
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
//        auto test = &
        std::function<YCode(void)>* run_function = (_loop_function == nullptr)
                                    ? &YThread::run
                                    : _loop_function;
        while (_running && !utils::exit_code(_test())) {}
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
    if (_running) { return YCode::ERR; }
    _running = true;
    YCode ret;
    _thread = std::thread([this,loop_function,ret]() {
        ret =
        while (_running && !utils::exit_code(loop_function())) {}
        _running = false;
    });
    _thread.detach();
    return ret;
}
