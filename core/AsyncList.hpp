#pragma once
#include <list>
#include <mutex>

namespace fpp {

    template <class Type>
    class AsyncList {

    public:

        AsyncList() = default;
        virtual ~AsyncList() { clear(); }

        void push_back(const Type& value) {
            std::lock_guard lock(_list_mutex);
            _list.push_back(value);
        }

        void push_back(const Type&& value) {
            std::lock_guard lock(_list_mutex);
            _list.push_back(value);
        }

        void remove_if(std::function<bool(const Type&)> condition) {
            std::lock_guard lock(_list_mutex);
            _list.remove_if(condition);
        }

    private:

        std::list<Type>     _list;
        std::mutex          _list_mutex;

    };

}
