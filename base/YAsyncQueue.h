#pragma once

#include <queue>
#include <mutex>

template <class Type>
class YAsyncQueue
{

public:

    YAsyncQueue() : _queue_capacity(100) {}
    ~YAsyncQueue() { clear(); }

    void push(Type data)
    {
        std::lock_guard<std::mutex> lock(_queue_mutex);
        _queue.push(data);
    }
    bool pop(Type& data)
    {
        std::lock_guard<std::mutex> lock(_queue_mutex);
        if (_queue.empty()) { return false; }
        data = _queue.front();
        _queue.pop();
        return true;
    }
    bool empty()
    {
        std::lock_guard<std::mutex> lock(_queue_mutex);
        return _queue.empty();
    }
    bool full()
    {
        std::lock_guard<std::mutex> lock(_queue_mutex);
        return _queue.size() >= _queue_capacity;
    }
    int64_t size()
    {
        std::lock_guard<std::mutex> lock(_queue_mutex);
        return _queue.size();
    }
    void clear()
    {
        std::lock_guard<std::mutex> lock(_queue_mutex);
        std::queue<Type> empty;
        std::swap(_queue, empty);
    }

private:

    std::queue<Type>        _queue;
    std::mutex              _queue_mutex;
    uint64_t                _queue_capacity;  //TODO

};

