#pragma once
#include "ffmpeg.h"

class YVideoRescaler
{

public:

    YVideoRescaler();

    bool init(AVCodecContext *input_codec_context, AVCodecContext *output_codec_context);

    bool rescale(AVFrame* frame);

};
