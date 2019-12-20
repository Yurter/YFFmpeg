//#pragma once
//#include <queue>
//#include <mutex>
//#include <atomic>
//#include <iostream>
//#include <condition_variable>

//namespace fpp {

//    template <class Type>
//    class AsyncDiscardSizeQueue {

//    public:

//        AsyncDiscardSizeQueue(uint64_t queue_capacity = 50 * 1024 * 1024 /* 50 Mb */) :
//            _queue_capacity(queue_capacity)
//            , _queue_size(0)
//            , _stop_wait(false) {}

//        virtual ~AsyncDiscardSizeQueue() { clear(); }

//        [[nodiscard]]
//        bool push(const Type& data) {
//            std::lock_guard<std::recursive_mutex> lock(_queue_mutex);
//            push_and_notify(data);
//            return true;
//        }

//        [[nodiscard]]
//        bool pop(Type& data) {
//            std::lock_guard<std::recursive_mutex> lock(_queue_mutex);
//            if (_queue.empty()) { return false; }
//            pop_and_notify(data);
//            return true;
//        }

//        [[nodiscard]]
//        bool wait_and_pop(Type& data) {
//            std::unique_lock<std::recursive_mutex> lock(_queue_mutex);
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
//            std::lock_guard<std::recursive_mutex> lock(_queue_mutex);
//            return _queue.empty();
//        }

//        bool full() {
//            std::lock_guard<std::recursive_mutex> lock(_queue_mutex);
//            return _queue.size() == _queue_capacity;
//        }

//        int64_t size() {
//            return _queue_size;
//        }

//        uint64_t length() {
//            std::lock_guard<std::recursive_mutex> lock(_queue_mutex);
//            return _queue.size();
//        }

//        void clear() {
//            std::lock_guard<std::recursive_mutex> lock(_queue_mutex);
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
//            while (notEnoughStorage(data.size())) {
//                std::cout << "DELETE " << data.size() << std::endl;
//                Type surplus_data;
//                pop_and_decrease_size(surplus_data);
//            }
//            push_and_increase_size(data);
//            _condition_variable_pushed.notify_one();
//        }

//        void pop_and_notify(Type& data) {
//            pop_and_decrease_size(data);
//            _condition_variable_popped.notify_one();
//        }

//        void push_and_increase_size(const Type& data) {
//            _queue.push(data);
//            _queue_size += data.size();
//        }

//        void pop_and_decrease_size(Type& data) {
//            data = Type(_queue.front());
//            _queue.pop();
//            _queue_size -= data.size();
//        }

//        bool not_enough_storage(uint64_t size) {
//            return (_queue_size + size) > _queue_capacity;
//        }

//    private:

//        std::queue<Type>            _queue;
//        std::recursive_mutex        _queue_mutex; //TODO вернуть обычный мьютекс
//        uint64_t                    _queue_capacity;
//        std::atomic_ullong          _queue_size;
//        std::atomic_bool            _stop_wait;
//        std::condition_variable_any _condition_variable_pushed;
//        std::condition_variable_any _condition_variable_popped;

//    };

//} // namespace fpp
