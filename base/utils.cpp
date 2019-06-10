#include "utils.h"
#include <thread>

std::string utils::media_type_to_string(YMediaType media_type)
{
    switch (media_type) {
    case YMediaType::MEDIA_TYPE_UNKNOWN:
        return "Unknown";
    case YMediaType::MEDIA_TYPE_VIDEO:
        return "Video";
    case YMediaType::MEDIA_TYPE_AUDIO:
        return "Audio";
    }
}

void utils::sleep_for(int64_t milliseconds)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

bool utils::exit_code(YCode code)
{
    if (code == YCode::ERR)             { return true; }
    if (code == YCode::NOT_INITED)      { return true; }
    if (code == YCode::END_OF_FILE)     { return true; }
    if (code == YCode::INVALID_INPUT)   { return true; }
    return false;
}

std::string utils::code_to_string(YCode code)
{
    if (code == YCode::OK)              { return "OK";              }
    if (code == YCode::ERR)             { return "Error";           }
    if (code == YCode::AGAIN)           { return "Again";           }
    if (code == YCode::NOT_INITED)      { return "Not inited";      }
    if (code == YCode::END_OF_FILE)     { return "EOF";             }
    if (code == YCode::INVALID_INPUT)   { return "Invalid input";   }
    return "Unknown error code: " + std::to_string(code);
}
