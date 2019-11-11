#pragma once
#include <functional>

namespace fpp {

    using LoopFunction = std::function<void(void)>;

    class Timer {


    public:

        Timer(std::fun);

        void start();
        void restart();
        void stop();

    };


} // namespace fpp
