#pragma once

#include <queue>
#include <mutex>

template <class Type>
class YAsyncQueue
{

public:

    YAsyncQueue();
    ~YAsyncQueue();

    void pushData(Type data)
    {
        std::lock_guard<std::mutex> lock(_queue_mutex);
        _queue.push(data);
    }
    bool popData(Type data)
    {
        std::lock_guard<std::mutex> lock(_queue_mutex);
        if (_queue.empty()) { return false; }
        data = _queue.front();
        _queue.pop();
        return true;
    }

private:

    std::queue<Type>        _queue;
    std::mutex              _queue_mutex;
    uint64_t                _queue_capacity;

};

