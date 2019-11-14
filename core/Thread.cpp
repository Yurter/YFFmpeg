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
        _running(false)
      , _stop_flag(false)
      , _exit_code(Code::OK)
      , _loop_function(loop_function)
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
//        try_throw(quit());
        try_throw(stop());
        join();
    }

    Code Thread::start() {
        return_if(running(), Code::INVALID_CALL_ORDER);
        _stop_flag = false;
        if_not(inited()) { try_to(init()); }
        _thread = std::thread([this]() {
            _running = true;
            try_throw(onStart());
            log_debug("Thread started");
            try {
                if_not (_stop_flag) {
                    do {
                        _exit_code = _loop_function();
                    } while_not (utils::exit_code(_exit_code));

                    std::string log_message = utils::error_code(_exit_code)
                            ? "Thread finished with code: "
                            : "Thread correctly finished with code: ";
                    log_info(log_message << _exit_code << " - " << utils::code_to_string(_exit_code));

                }
            }
            catch (std::exception e) {
                log_error("std::exception: " << e.what());
                _exit_code = Code::EXCEPTION;
                _exit_message = e.what();
            }
            catch (...) {
                log_error("unknown exception!");
                _exit_code = Code::EXCEPTION;
                _exit_message = "unknown";
            }
            _running = false;
            try_throw(stop());
        });
        _thread.detach(); //TODO убрать? без детача падения на выходе, выяснить в каком объекте и почему
        return Code::OK;
    }

    Code Thread::stop() {
        _stop_flag = true;
        try_to(quit());
        try_to(onStop());
        return Code::OK;
    }

    Code Thread::quit() {
        return_if_not(running(), Code::OK);
        try { /* TODO */
            if (_thread.joinable()) { _thread.join(); }
        } catch (std::exception e) {
            std::cout << "TODO: exeption: " << e.what() << std::endl;
            _exit_message = e.what();
            return Code::EXCEPTION;
        }
        return Code::OK;
    }

    bool Thread::running() const {
        return _running;
    }

    void Thread::join() const {
//        while (running()) { utils::sleep_for(MEDIUM_DELAY_MS); }
        while (running()) {
            log_warning("still running");
            utils::sleep_for(MEDIUM_DELAY_MS);
        }
    }

    Code Thread::exitCode() const {
        return _exit_code;
    }

    Code Thread::run() {
        return Code::NOT_IMPLEMENTED;
    }

    Code Thread::onStart() {
        return Code::OK;
    }

    Code Thread::onStop() {
        return Code::OK;
    }

} // namespace fpp
