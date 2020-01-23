#include "Exception.hpp"

namespace fpp {

    Exception::Exception(const std::string& error_message)
        : _error_message(error_message) {
    }

    const char* Exception::what() const noexcept {
        return _error_message.c_str();
    }

} // namespace fpp
