#pragma once

#include "base/ffmpeg.h"
#include "base/YAbstractRefi.h"
#include "../control/YVideoStream.h"

class YRescaler : public YAbstractRefi
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

