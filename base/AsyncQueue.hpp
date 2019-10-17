#pragma once
#include <queue>
#include <mutex>
#include <atomic>

namespace fpp {

    template <class Type>
    class AsyncQueue {

    public:

        AsyncQueue(uint64_t queue_capacity = 1'000) :
            _queue_capacity(queue_capacity)
            , _stop_wait(false) {}
        virtual ~AsyncQueue() { clear(); }

        [[nodiscard]]
        bool push(const Type& data) {
            std::lock_guard<std::mutex> lock(_queue_mutex);
            if (_queue.size() == _queue_capacity) {
                return false;
            }
            _queue.push(data);
            _condition_variable_pushed.notify_one();
            return true;
        }

        [[nodiscard]]
        bool pop(Type& data) {
            std::lock_guard<std::mutex> lock(_queue_mutex);
            if (_queue.empty()) { return false; }
            data = _queue.front();
            _queue.pop();
            return true;
        }

        [[nodiscard]]
        bool wait_and_pop(Type& data) {
            std::unique_lock<std::mutex> lock(_queue_mutex);
            if (!_queue.empty()) {
                data = _queue.front();
                _queue.pop();
                return true;
            }

            _condition_variable_pushed.wait(lock, [&](){
                return !_queue.empty() || _stop_wait;
            });

            if (_stop_wait) {
                _stop_wait = false;
                return false;
            }

            data = _queue.front();
            _queue.pop();
            return true;
        }

        bool empty() {
            std::lock_guard<std::mutex> lock(_queue_mutex);
            return _queue.empty();
        }

        bool full() {
            std::lock_guard<std::mutex> lock(_queue_mutex);
            return _queue.size() == _queue_capacity;
        }

        int64_t size() {
            std::lock_guard<std::mutex> lock(_queue_mutex);
            return _queue.size();
        }

        void clear() {
            std::lock_guard<std::mutex> lock(_queue_mutex);
            std::queue<Type> empty;
            std::swap(_queue, empty);
        }

        void setCapacity(uint64_t queue_capacity) {
            _queue_capacity = queue_capacity;
        }

        void stop_wait() {
            _stop_wait = true;
            _condition_variable_pushed.notify_one();
        }

    private:

        std::queue<Type>        _queue;
        std::mutex              _queue_mutex;
        uint64_t                _queue_capacity;
        std::atomic_bool        _stop_wait;
        std::condition_variable _condition_variable_pushed;

    };

} // namespace fpp
