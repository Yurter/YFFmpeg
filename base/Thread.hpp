#pragma once
#include <thread>
#include <functional>
#include <atomic>
#include "Object.hpp"

namespace fpp {

    //typedef std::function<Code(void)> LoopFunction;
    using LoopFunction = std::function<Code(void)>;

    class Thread : public Object {

    public:

        Thread();
        Thread(LoopFunction loop_function);
        Thread& operator=(Thread&& other);
        Thread(const Thread&)               = delete;
        Thread& operator=(const Thread&)    = delete;
        virtual ~Thread() override;

        Code                start();
        Code                stop();
        Code                quit();
        bool                running();
        void                join();

        Code                exitCode()      const;
        std::string         exitMessage()   const;

    protected:

        virtual Code        run();
        virtual Code        onStop();

    private:

        std::thread         _thread;
        std::atomic_bool    _running;
        Code                _exit_code;
        std::string         _exit_message;
        LoopFunction        _loop_function;

    };

} // namespace fpp
