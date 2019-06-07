#pragma once

#include "base/ffmpeg.h"

class YRescaler
{

public:

    YRescaler();

    bool init(AVCodecContext *input_codec_context, AVCodecContext *output_codec_context);

    bool rescale(AVFrame* frame);

};
