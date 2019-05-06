#pragma once

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
#include <libavutil/common.h>
#include <libavutil/imgutils.h>
#include <libavutil/samplefmt.h>
#include <libavutil/mathematics.h>
#include <libavutil/channel_layout.h>
}

class YAbstractCoder
{

public:

    YAbstractCoder();
    virtual ~YAbstractCoder();

    virtual bool init() = 0;

protected:

    virtual bool initVideoCodec() = 0;
    virtual bool initAudioCodec() = 0;

protected:

    // General parameters

    // FFmpeg
    AVCodec				*_video_codec;
    AVCodec				*_audio_codec;
    AVCodecContext		*_video_codec_context;
    AVCodecContext		*_audio_codec_context;

    // Media parameters


};
