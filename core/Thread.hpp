#pragma once
#include <thread>
#include <functional>
#include <atomic>
#include "Object.hpp"

namespace fpp {

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
        bool                running() const;
        void                join() const;

        Code                exitCode()      const;
        std::string         exitMessage()   const;

    protected:

        virtual Code        run();
        virtual Code        onStart();  //вызывается из потока перед основным циклом
        virtual Code        onStop();   //Должен гарантированно вызыватся после остановки потока

    private:

        std::thread         _thread;
        std::atomic_bool    _running;
        std::atomic_bool    _stop_flag;
        Code                _exit_code;
        std::string         _exit_message;
        LoopFunction        _loop_function;

    };

} // namespace fpp
