#pragma once
#include <exception>
#include <string>

namespace fpp {

    class Exception : public std::exception {

    public:

        Exception(const std::string& error_message);

};

} // namespace fpp
