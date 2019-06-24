#pragma once

#include "base/ffmpeg.h"
#include "base/YAbstractRefi.h"
#include "../control/YVideoStream.h"

class YRescaler : public YAbstractRefi
{

public:

    YRescaler(streams_pair audio_streams);

    YCode init() override;

    bool rescale(AVFrame* frame);

    YCode processInputData(YFrame& input_data) override;
};

