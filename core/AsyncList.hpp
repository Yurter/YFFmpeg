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

        bool empty() const {
            std::lock_guard<std::mutex> lock(_list_mutex);
            return _list.empty();
        }

        auto for_each(std::function<void(const Type&)> foo) {
            std::lock_guard<std::mutex> lock(_list_mutex);
            for (const auto& item : _list) {
                foo(item);
            }
        }

//        auto begin() {
//            return _list.begin();
//        }
//        auto end() {
//            return _list.end();
//        }

    private:

        std::list<Type>     _list;
        std::mutex          _list_mutex;

    };

}
