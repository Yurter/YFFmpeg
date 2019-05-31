#pragma once

#include "ffmpeg.h"
#include <string>

class utils
{

public:

    utils();
    ~utils();

    static std::string mediaTypeToString(YMediaType media_type);
};
