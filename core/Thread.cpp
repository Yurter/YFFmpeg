#include "Thread.hpp"
#include "utils.hpp"
#include <exception>

namespace fpp {

    Thread::Thread() :
        Thread(std::bind(&Thread::run,this))
    {
        EMPTY_CONSTRUCTOR
    }

    Thread::Thread(LoopFunction loop_function) :
        _running(false),
        _exit_code(Code::OK),
        _loop_function(loop_function)
    {
        setName("Thread");
    }

    Thread& Thread::operator=(Thread&& other) {
        _thread         = std::move(other._thread);
        _running        = other._running.load();
        _loop_function  = other._loop_function;
        return *this;
    }

    Thread::~Thread() {
        try_throw(quit());
    }

    Code Thread::start() {
        return_if(running(), Code::INVALID_CALL_ORDER);
        if_not(inited()) { try_to(init()); }
        _running = true;
        _thread = std::thread([this]() {
            try {
                log_debug("Thread started");
                while (_running && !utils::exit_code(_exit_code = _loop_function())) {}
                if (utils::error_code(_exit_code)) {
                    log_error("Thread finished with code: " << _exit_code
                              << " - " << utils::code_to_string(_exit_code));
                } else {
                    log_info("Thread correctly finished with code: " << _exit_code
                              << " - " << utils::code_to_string(_exit_code));
                }
                _running = false;
            }
            catch (std::exception e) {
                log_error("std::exception: " << e.what());
                _exit_code = Code::EXCEPTION;
                _exit_message = e.what();
                return;
            }
            catch (...) {
                log_error("unknown exception!");
                _exit_code = Code::EXCEPTION;
                _exit_message = "unknown";
                return;
            }
        });
        _thread.detach(); //TODO убрать?
        return Code::OK;
    }

    Code Thread::stop() {
        try_to(quit());
        try_to(onStop());
        return Code::OK;
    }

    //TODO join?
    Code Thread::quit() {
        return_if_not(running(), Code::INVALID_CALL_ORDER);
        _running = false;
        try { /* TODO */
            if (_thread.joinable()) { _thread.join(); }
        } catch (std::exception e) {
            std::cout << "TODO: exeption: " << e.what() << std::endl;
            _exit_message = e.what();
            return Code::EXCEPTION;
        }
        return Code::OK;
    }

    bool Thread::running() {
        return _running;
    }

    void Thread::join() {
        while (running()) { utils::sleep_for(MEDIUM_DELAY_MS); }
    }

    Code Thread::exitCode() const {
        return _exit_code;
    }

    Code Thread::run() {
        return Code::NOT_INITED;
    }

    Code Thread::onStart() {
        return Code::NOT_INITED;
    }

    Code Thread::onStop() {
        return Code::NOT_INITED;
    }

} // namespace fpp
