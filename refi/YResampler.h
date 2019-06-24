#pragma once

#include "base/YAbstractRefi.h"
#include "../control/YAudioStream.h"

class YResampler : public YAbstractRefi
{

public:

    YResampler(streams_pair audio_streams);
    virtual ~YResampler() override;

    virtual YCode       init() override;

private:

    virtual YCode       processInputData(YFrame& input_data) override;
    bool                initOutputFrame(AVFrame **frame, int frame_size);
    bool                configChanged(AVFrame *in, AVFrame *out);
    void                stampFrame(AVFrame *frame); //TODO нужно ли?

protected:

    // General
    int64_t             _frame_pts; //TODO нужно ли?

    // FFmpeg
    SwrContext*         _resampler_context;

};
