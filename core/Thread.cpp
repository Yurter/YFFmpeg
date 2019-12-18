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
        try_throw(quit());
        join();
    }

    Code Thread::start() {
        return_if(running(), Code::OK);
//        log_trace("Thread starting."); //блокирует конструктор логгера.
        _stop_flag = false;
        if_not(inited()) { try_to(init()); }
        _running = true; // TODO вынес из тела потока, т.к. может не успеть установиться в true из потока
        _thread = std::thread([this]() {
//            _running = true;
            log_debug("Thread started");
            try_throw(onStart());
            try {
                do {
                    if (_stop_flag) { break; }
                    _exit_code = _loop_function();
                } while_not (utils::exit_code(_exit_code));

                std::string log_message = utils::error_code(_exit_code)
                        ? "Thread finished with code: "
                        : "Thread correctly finished with code: ";
                log_debug(log_message << _exit_code << " - " << utils::code_to_string(_exit_code));
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
            log_debug("Thread finished");
        });
        _thread.detach(); //TODO убрать? без детача падения на выходе, выяснить в каком объекте и почему
        return Code::OK;
    }

    Code Thread::stop() {
        log_debug("Thread stoping");
        _stop_flag = true;
        try_to(quit());
        try_to(onStop());
        return Code::OK;
    }

    Code Thread::quit() {
        return_if_not(running(), Code::OK);
        log_debug("Thread quiting");
        _stop_flag = true; //TODO дублировать из стопа?
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
        log_debug("Thread joining");
        while (running()) {
            log_debug("Waiting for " << this->name() << "'s thread...");
            utils::sleep_for(MEDIUM_DELAY_MS);
        }
        log_debug("Thread joined");
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
