#pragma once

#include "base/YRefi.h"
#include "../control/YAudioStream.h"

class YResampler : public YRefi
{

public:

    YResampler(streams_pair audio_streams);
    virtual ~YResampler() override;

    virtual YCode       init() override;

private:

    virtual YCode       processInputData(YFrame& input_data) override;
    bool                initOutputFrame(AVFrame** frame, int frame_size);
    bool                configChanged(AVFrame* in, AVFrame* out);

protected:

    // FFmpeg
    SwrContext*         _resampler_context;

};
