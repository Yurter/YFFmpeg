#include "Exception.hpp"

namespace fpp {

    Exception::Exception(const std::string& error_message)
        : std::exception(error_message.c_str()) {
    }

} // namespace fpp
