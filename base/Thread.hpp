#pragma once
#include <thread>
#include <functional>
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

        virtual Code        start()     final;
        virtual Code        quit()      final;
        virtual bool        running()   final;
        virtual void        join()      final;

        Code                exitCode()      const;
        std::string         exitMessage()   const;

    protected:

        virtual Code        run();

    private:

        std::thread         _thread;
        volatile bool       _running;
        Code                _exit_code;
        std::string         _exit_message;
        LoopFunction        _loop_function;

    };

} // namespace fpp
