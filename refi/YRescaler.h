#pragma once

#include "base/ffmpeg.h"
#include "base/YAbstractFrameProcessor.h"
#include "../control/YVideoStream.h"

class YRescaler : public YAbstractFrameProcessor
{

public:

    YRescaler(streams_pair audio_streams);

    YCode init() override;

    bool rescale(AVFrame* frame);

    YCode processInputData(YFrame& input_data) override;
};

