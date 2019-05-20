#pragma once

#include "ffmpeg.h"

class YAudioResampler
{

public:

    YAudioResampler();
    ~YAudioResampler();

    bool init(AVCodecContext *input_codec_context, AVCodecContext *output_codec_context);

    bool resample(AVFrame* frame);

protected:

    // General parameters
    bool                _inited;

    // FFmpeg
    SwrContext*         _resample_context;

    // Media parameters
};
