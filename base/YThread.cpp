#include "YThread.hpp"
#include "utils.hpp"
#include <exception>

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
    if_not(inited()) { try_to(init()); }
    _running = true;
    _thread = std::thread([this]() {
        try {
            log_debug("Thread started");
            while (_running && !utils::exit_code(_exit_code = _loop_function())) {}
            if (utils::exit_code(_exit_code)) {
                log_error("Thread finished with code: " << _exit_code
                          << " - " << utils::code_to_string(_exit_code));
            } else {
                log_debug("Thread correctly finished with code: " << _exit_code
                          << " - " << utils::code_to_string(_exit_code));
            }
            _running = false;
        } catch (std::exception e) {
            _exit_code = YCode::EXCEPTION;
            _exit_message = e.what();
            return;
        }
    });
    _thread.detach(); //TODO убрать?
    return YCode::OK;
}

YCode YThread::quit() //TODO join?
{
    return_if_not(running(), YCode::INVALID_CALL_ORDER);
    _running = false;
    try { /* TODO */
        if (_thread.joinable()) { _thread.join(); }
    } catch (std::exception e) {
        std::cout << "TODO: exeption: " << e.what() << std::endl;
        _exit_message = e.what();
        return YCode::EXCEPTION;
    }
    return YCode::OK;
}

bool YThread::running()
{
    return _running;
}

void YThread::join()
{
    while (running()) { utils::sleep_for(MEDIUM_DELAY_MS); }
}

YCode YThread::exitCode() const
{
    return _exit_code;
}

YCode YThread::run()
{
    return YCode::NOT_INITED;
}
