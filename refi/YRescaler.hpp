#pragma once

#include "base/ffmpeg.hpp"
#include "base/YRefi.hpp"
#include "../control/YVideoStream.hpp"

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

