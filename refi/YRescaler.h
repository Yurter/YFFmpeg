#pragma once

#include "base/ffmpeg.h"
#include "base/YRefi.h"
#include "../control/YVideoStream.h"

class YRescaler : public YRefi
{

public:

    YRescaler(streams_pair audio_streams);
    virtual ~YRescaler() override;

    YCode               init() override;
    YCode               processInputData(YFrame& input_data) override;

private:

    //FFmpeg
    //

};

