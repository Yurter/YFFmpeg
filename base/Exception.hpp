#pragma once
#include "Object.hpp"
#include <exception>

namespace fpp {

    class Exception : public Object, public std::exception { //TODO

    public:

        Exception(std::string error_message);

};

} // namespace fpp
