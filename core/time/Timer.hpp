#pragma once
#include <functional>
#include <thread>
#include <chrono>
#include "core/async/AsyncQueue.hpp"

namespace fpp {

//    using DelayedFunction = std::function<void()>;
//    using ExecuteTime = std::chrono::time_point<std::chrono::steady_clock>;
//    using ScheduledFunction = std::pair<DelayedFunction, ExecuteTime>;

    struct DelayedFunction {
        std::function<void()>   function;
        int                     delay;
    };

    class Timer {

    public:

        Timer() {
            _thread = std::thread([&]() {
                DelayedFunction delayed_function;
//                while (_function_queue.wait_and_pop(delayed_function)) {
//                    std::this_thread::sleep_for(
//                        std::chrono::milliseconds(delayed_function.delay)
//                    );
//                }
            });
        }

        ~Timer() {
//            _function_queue.clear();
//            _function_queue.stop_wait();
            if (_thread.joinable()) {
                _thread.join();
            }
        }

        void setTimeout(std::function<void()> function, int delay) {
//            if (!_function_queue.push({ function, delay })) {
//                //
//            }
        }

        void setInterval(DelayedFunction function, int interval) {
            //
        }

//        void                start(int64_t msec);
//        void                restart();
//        void                stop();
//        int64_t             interval() const;
//        bool                isActive() const;
//        bool                isSingleShot() const;
//        int64_t             remainingTime() const;
//        void                setInterval(int msec);
//        void                setSingleShot(bool singleShot);

    private:

        using FunctionsQueue = AsyncQueue<DelayedFunction>;

//        FunctionsQueue      _function_queue;
        std::thread         _thread;

    };


} // namespace fpp
