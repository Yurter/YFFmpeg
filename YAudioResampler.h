#pragma once

#include "base/ffmpeg.h"

class YAudioResampler
{

public:

    YAudioResampler();
    ~YAudioResampler();

    bool init(AVCodecContext *input_codec_context, AVCodecContext *output_codec_context);
    bool resample(AVFrame **frame);

private:

    bool initOutputFrame(AVFrame **frame, int frame_size);
    bool configChanged(const AVFrame *in, const AVFrame *out);
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
