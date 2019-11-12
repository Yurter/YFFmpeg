#pragma once
#include <functional>

namespace fpp {

    template<typename FunctionType>
    class Timer {

    public:

        Timer(FunctionType function) {
            // TODO
        }
        ~Timer() {
            // TODO
        }

        void                start(int64_t msec);
        void                restart();
        void                stop();


        int64_t             interval() const;
        bool                isActive() const;
        bool                isSingleShot() const;
        int64_t             remainingTime() const;
        void                setInterval(int msec);
        void                setSingleShot(bool singleShot);

    private:

        FunctionType        _function;

    };


} // namespace fpp
