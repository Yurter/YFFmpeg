#pragma once

#include "ffmpeg.h"
#include <string>

class utils
{

public:

    static std::string  media_type_to_string(YMediaType media_type);
    static void         sleep_for(int64_t milliseconds);
    static bool         exit_code(YCode code);
    static std::string  code_to_string(YCode code);

};
