#pragma once

#include "base/YRefi.hpp"
#include "../control/YAudioStream.hpp"

class YResampler : public YRefi
{

public:

    YResampler(StreamPair audio_streams);
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
