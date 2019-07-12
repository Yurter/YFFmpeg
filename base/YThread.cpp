#include "YThread.h"
#include "utils.h"

YThread::YThread() :
    YThread(std::bind(&YThread::run,this))
{
    EMPTY_CONSTRUCTOR
}

YThread::YThread(LoopFunction loop_function) :
    _running(false),
    _exit_code(YCode::NOT_INITED),
    _loop_function(loop_function)
{
    setName("YThread");
}

YThread& YThread::operator=(YThread&& other)
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

YCode YThread::start()
{
    return_if(running(), YCode::INVALID_CALL_ORDER);
    return_if_not(inited(), YCode::NOT_INITED);
    _thread = std::thread([this]() {
        log_debug("Thread started");
        _running = true;
        while (_running && !utils::exit_code(_exit_code = _loop_function())) {}
        _running = false;
        log_debug("Thread finished with code: " << _exit_code
                  << " - " << utils::code_to_string(_exit_code));
    });
    _thread.detach();
    return YCode::OK;
}

void YThread::quit() //TODO join?
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
    while (running()) { utils::sleep_for(MEDIUM_DELAY_MS); }
}

void YThread::setExitCode(YCode exit_code)
{
    _exit_code = exit_code;
}

YCode YThread::exitCode() const
{
    return _exit_code;
}

YCode YThread::run()
{
    return YCode::NOT_INITED;
}
