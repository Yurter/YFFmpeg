#pragma once

#include "ffmpeg.h"
#include <string>

class utils
{

public:

    utils() = default;
    ~utils() = default;

    inline static std::string   media_type_to_string(YMediaType media_type);
    inline static void          sleep_for(int64_t milliseconds);
};
