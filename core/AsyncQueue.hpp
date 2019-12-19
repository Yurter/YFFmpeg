#pragma once
#include "AsyncObject.hpp"
#include <queue>
#include <atomic>

namespace fpp {

    template <class T>
    class AsyncQueue : public AsyncObject<T> {

    public:

        AsyncQueue(uint64_t queue_capacity = 50) :
            _queue_capacity(queue_capacity)
            , _stop_wait(false) {}

        virtual ~AsyncQueue() override { stop_wait(); }

        [[nodiscard]]
        bool push(const T& data) {
            if (full()) {
                return false;
            }
            push_and_notify(data);
            return true;
        }

        [[nodiscard]]
        bool pop(Type& data) {
            std::lock_guard<std::mutex> lock(_queue_mutex);
            if (_queue.empty()) { return false; }
            pop_and_notify(data);
            return true;
        }

        [[nodiscard]]
        bool wait_and_push(const Type& data) {
            std::unique_lock<std::mutex> lock(_queue_mutex);
            if (push_available()) {
                push_and_notify(data);
                return true;
            }

            _condition_variable_popped.wait(lock, [&]() {
                return push_available() || _stop_wait;
            });

            if (_stop_wait) {
                _stop_wait = false;
                return false;
            }

            push_and_notify(data);
            return true;
        }

        [[nodiscard]]
        bool wait_and_pop(Type& data) {
            std::unique_lock<std::mutex> lock(_queue_mutex);
            if (!_queue.empty()) {
                pop_and_notify(data);
                return true;
            }

            _condition_variable_pushed.wait(lock, [&]() {
                return !_queue.empty() || _stop_wait;
            });

            if (_stop_wait) {
                _stop_wait = false;
                return false;
            }

            pop_and_notify(data);
            return true;
        }

        bool empty() {
            std::lock_guard<std::mutex> lock(_queue_mutex);
            return _queue.empty();
        }

        int64_t lenght() {
            std::lock_guard<std::mutex> lock(_queue_mutex);
            return _queue.size();
        }

        void clear() {
            std::lock_guard<std::mutex> lock(_queue_mutex);
            std::queue<Type> empty;
            std::swap(_queue, empty);
        }

        void set_capacity(uint64_t queue_capacity) {
            _queue_capacity = queue_capacity;
        }

        void stop_wait() {
            _stop_wait = true;
            _condition_variable_pushed.notify_one();
            _condition_variable_popped.notify_one();
        }

    private:

        void push_and_notify(const Type& data) {
            _queue.push(data);
            _condition_variable_pushed.notify_one();
        }

        void pop_and_notify(Type& data) {
            data = Type(_queue.front());
            _queue.pop();
            _condition_variable_popped.notify_one();
        }

        bool push_available() const {
            return _queue.size() < _queue_capacity;
        }

        bool full() {
            return _queue.size() == _queue_capacity;
        }

    private:

        uint64_t                _queue_capacity;
        std::atomic_bool        _stop_wait;
        std::condition_variable _condition_variable_pushed;
        std::condition_variable _condition_variable_popped;

    };

} // namespace fpp


///////////////////////////////////////////////////////////////////////////
//#pragma once
//#include <queue>
//#include <mutex>
//#include <atomic>

//namespace fpp {

//    template <class Type>
//    class AsyncQueue {

//    public:

//        AsyncQueue(uint64_t queue_capacity = 50) :
//            _queue_capacity(queue_capacity)
//            , _stop_wait(false) {}
//        virtual ~AsyncQueue() { clear(); }

//        [[nodiscard]]
//        bool push(const Type& data) {
//            std::lock_guard<std::mutex> lock(_queue_mutex);
//            if (full()) {
//                return false;
//            }
//            push_and_notify(data);
//            return true;
//        }

//        [[nodiscard]]
//        bool pop(Type& data) {
//            std::lock_guard<std::mutex> lock(_queue_mutex);
//            if (_queue.empty()) { return false; }
//            pop_and_notify(data);
//            return true;
//        }

//        [[nodiscard]]
//        bool wait_and_push(const Type& data) {
//            std::unique_lock<std::mutex> lock(_queue_mutex);
//            if (push_available()) {
//                push_and_notify(data);
//                return true;
//            }

//            _condition_variable_popped.wait(lock, [&]() {
//                return push_available() || _stop_wait;
//            });

//            if (_stop_wait) {
//                _stop_wait = false;
//                return false;
//            }

//            push_and_notify(data);
//            return true;
//        }

//        [[nodiscard]]
//        bool wait_and_pop(Type& data) {
//            std::unique_lock<std::mutex> lock(_queue_mutex);
//            if (!_queue.empty()) {
//                pop_and_notify(data);
//                return true;
//            }

//            _condition_variable_pushed.wait(lock, [&]() {
//                return !_queue.empty() || _stop_wait;
//            });

//            if (_stop_wait) {
//                _stop_wait = false;
//                return false;
//            }

//            pop_and_notify(data);
//            return true;
//        }

//        bool empty() {
//            std::lock_guard<std::mutex> lock(_queue_mutex);
//            return _queue.empty();
//        }

//        int64_t lenght() {
//            std::lock_guard<std::mutex> lock(_queue_mutex);
//            return _queue.size();
//        }

//        void clear() {
//            std::lock_guard<std::mutex> lock(_queue_mutex);
//            std::queue<Type> empty;
//            std::swap(_queue, empty);
//        }

//        void set_capacity(uint64_t queue_capacity) {
//            _queue_capacity = queue_capacity;
//        }

//        void stop_wait() {
//            _stop_wait = true;
//            _condition_variable_pushed.notify_one();
//            _condition_variable_popped.notify_one();
//        }

//    private:

//        void push_and_notify(const Type& data) {
//            _queue.push(data);
//            _condition_variable_pushed.notify_one();
//        }

//        void pop_and_notify(Type& data) {
//            data = Type(_queue.front());
//            _queue.pop();
//            _condition_variable_popped.notify_one();
//        }

//        bool push_available() const {
//            return _queue.size() < _queue_capacity;
//        }

//        bool full() {
//            return _queue.size() == _queue_capacity;
//        }

//    private:

//        std::queue<Type>        _queue;
//        std::mutex              _queue_mutex;
//        uint64_t                _queue_capacity;
//        std::atomic_bool        _stop_wait;
//        std::condition_variable _condition_variable_pushed;
//        std::condition_variable _condition_variable_popped;

//    };

//} // namespace fpp
