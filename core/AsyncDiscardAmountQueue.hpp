#pragma once
#include <queue>
#include <mutex>
#include <atomic>
#include <iostream>
#include <condition_variable>

namespace fpp {

    template <class Type>
    class AsyncDiscardAmountQueue {

    public:

        AsyncDiscardAmountQueue(uint64_t queue_capacity = 50) :
            _queue_capacity(queue_capacity)
            , _stop_wait(false) {}

        virtual ~AsyncDiscardAmountQueue() { clear(); }

        [[nodiscard]]
        bool push(const Type& data) {
            std::lock_guard lock(_queue_mutex);
            push_and_notify(data);
            return true;
        }

        [[nodiscard]]
        bool pop(Type& data) {
            std::lock_guard lock(_queue_mutex);
            if (_queue.empty()) { return false; }
            pop_and_notify(data);
            return true;
        }

        [[nodiscard]]
        bool wait_and_pop(Type& data) {
            std::unique_lock lock(_queue_mutex);
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

        void pop_while(std::function<bool(const Type&)>&& pred) {
            std::lock_guard lock(_queue_mutex);
            while (pred(_queue.front())) {
                _queue.pop();
            }
        }

        auto front() {
            std::lock_guard lock(_queue_mutex);
            return _queue.front();
        }

        bool empty() {
            std::lock_guard lock(_queue_mutex);
            return _queue.empty();
        }

        bool full() {
            std::lock_guard lock(_queue_mutex);
            return _queue.size() == _queue_capacity;
        }

        uint64_t length() {
            std::lock_guard lock(_queue_mutex);
            return _queue.size();
        }

        void clear() {
            std::lock_guard lock(_queue_mutex);
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
            while (notEnoughStorage()) {
                _queue.pop();
            }
            _queue.push(data);
            _condition_variable_pushed.notify_one();
        }

        void pop_and_notify(Type& data) {
            data = Type(_queue.front());
            _queue.pop();
            _condition_variable_popped.notify_one();
        }

        bool notEnoughStorage() {
            return _queue.size() >= _queue_capacity;
        }

    private:

        std::queue<Type>        _queue;
        std::mutex              _queue_mutex;
        uint64_t                _queue_capacity;
        std::atomic_bool        _stop_wait;
        std::condition_variable _condition_variable_pushed;
        std::condition_variable _condition_variable_popped;

    };

} // namespace fpp
