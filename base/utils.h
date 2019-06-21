#pragma once

#include "ffmpeg.h"
#include "logger.h"
#include <string>

class utils
{

public:

    static std::string  media_type_to_string(YMediaType media_type);
    static void         sleep_for(int64_t milliseconds);
    static bool         exit_code(YCode code);
    static std::string  code_to_string(YCode code);
    static std::string  rational_to_string(AVRational rational);
    static bool         compatibleWithSampleFormat(AVCodecContext* codec_context, AVSampleFormat sample_format);
    static AVMediaType  ymedia_type_to_avmedia_type(YMediaType media_type);
    static int64_t      gen_stream_uid(int64_t context_index, int64_t stream_index);

};
