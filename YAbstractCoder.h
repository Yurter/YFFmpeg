#pragma once

#include "ffmpeg.h"

class YAbstractCoder
{

public:

    YAbstractCoder();
    virtual ~YAbstractCoder();

    virtual bool        init() = 0;

    AVCodecContext*     videoCodecContext();
    AVCodecContext*     audioCodecContext();

protected:

    virtual bool        initVideoCodec() = 0;
    virtual bool        initAudioCodec() = 0;

protected:

    // General parameters

    // FFmpeg
    AVCodecContext		*_video_codec_context;
    AVCodecContext		*_audio_codec_context;

};
