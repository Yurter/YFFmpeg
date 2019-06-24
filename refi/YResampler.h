#pragma once

#include "base/ffmpeg.h"
#include "base/YAbstractRefi.h"
#include "../control/YAudioStream.h"

class YResampler : public YAbstractRefi
{

public:

    YResampler(streams_pair audio_streams);
    virtual ~YResampler() override;

    virtual YCode init() override;

private:

    virtual YCode processInputData(YFrame& input_data) override;
    bool initOutputFrame(AVFrame **frame, int frame_size);
    bool configChanged(AVFrame *in, AVFrame *out);
    void stampFrame(AVFrame *frame);

protected:

    // General parameters
    streams_pair        _audio_streams;
    int64_t             _frame_pts;

    // FFmpeg
    SwrContext*         _resampler_context;

};
