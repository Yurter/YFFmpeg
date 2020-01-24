#include "FFmpegException.hpp"

namespace fpp {

    FFmpegException::FFmpegException(const std::string& error_message, int ret)
        : _error_message(error_message)
        , _ret(ret) {
    }

    const char* FFmpegException::what() const noexcept {
        return _error_message.c_str();
    }

    int FFmpegException::ret() const noexcept {
        return _ret;
    }

} // namespace fpp
