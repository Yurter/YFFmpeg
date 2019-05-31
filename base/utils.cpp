#include "utils.h"

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
