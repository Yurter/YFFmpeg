#pragma once

#include "ffmpeg.h"
#include <string>

class YAbstractCoder
{

public:

    YAbstractCoder();
    virtual ~YAbstractCoder();

    virtual bool        init() = 0;

//    void setOption(std::string name, std::string value);
//    void setOption(std::string name, int64_t     value);
//    void setOption(std::string name, double      value);

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
