#pragma once

#include "base/ffmpeg.h"
#include "base/YAbstractFrameProcessor.h"

class YRescaler : public YAbstractFrameProcessor
{

public:

    YRescaler();

    bool init(AVCodecContext *input_codec_context, AVCodecContext *output_codec_context);

    bool rescale(AVFrame* frame);

};
