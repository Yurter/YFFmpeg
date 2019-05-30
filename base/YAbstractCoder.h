#pragma once

#include "ffmpeg.h"
#include <string>
#include <list>

class YAbstractCoder
{

public:

    YAbstractCoder();
    virtual ~YAbstractCoder();

    virtual bool        init() = 0;

    AVCodecContext*     videoCodecContext();
    AVCodecContext*     audioCodecContext();

    bool                compatibleWithSampleFormat(AVSampleFormat sample_format);

protected:

    virtual bool        initVideoCodec() = 0;
    virtual bool        initAudioCodec() = 0;

protected:

    // General parameters
    int64_t             _video_stream_index;
    int64_t             _audio_stream_index;

    // FFmpeg
    AVCodecContext*     _video_codec_context;
    AVCodecContext*     _audio_codec_context;

};
