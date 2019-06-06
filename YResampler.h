#pragma once

#include "base/ffmpeg.h"
#include "base/YFrame.h"
#include "base/YAbstractFrameProcessor.h"

class YAudioResampler : public YAbstractFrameProcessor
{

public:

    YAudioResampler();
    ~YAudioResampler() override;

    bool init(AVCodecContext *input_codec_context, AVCodecContext *output_codec_context);

private:

    YCode run() override;
    bool initOutputFrame(AVFrame **frame, int frame_size);
    bool configChanged(AVFrame *in, AVFrame *out);
    void stampFrame(AVFrame *frame);

protected:

    // General parameters
    bool                _inited;

    int64_t             _frame_pts;

    // FFmpeg
    AVCodecContext*     _input_codec_context;
    AVCodecContext*     _output_codec_context;
    SwrContext*         _resampler_context;
    AVAudioFifo*        _audio_fifo;

    // Media parameters
    //

};
