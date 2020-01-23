#pragma once
#include <exception>
#include <string>

namespace fpp {

    class Exception : public std::exception {

    public:

        Exception(const std::string& error_message);
        virtual ~Exception() override = default;

        virtual const char* what() const noexcept override;

    protected:

        const std::string _error_message;

    };

} // namespace fpp
