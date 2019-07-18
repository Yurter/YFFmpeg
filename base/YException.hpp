#pragma once
#include "YObject.hpp"
#include <exception>

class YException : public YObject, public std::exception
{

public:

    YException(std::string error_message);

    //
};

