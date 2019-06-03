#pragma once

#include "base/ffmpeg.h"
#include "base/YThread.h"
#include "base/YFrame.h"
#include "base/YAsyncQueue.h"

class YAudioResampler : public YThread
{

public:

    YAudioResampler();
    ~YAudioResampler() override;

    void    push(YFrame frame);
    bool    pop(YFrame& frame);

    bool init(AVCodecContext *input_codec_context, AVCodecContext *output_codec_context);
//    bool resample(AVFrame **frame);

private:

    YCode run() override;
    bool initOutputFrame(AVFrame **frame, int frame_size);
    bool configChanged(const AVFrame *in, const AVFrame *out);
    void stampFrame(AVFrame *frame);

protected:

    // General parameters
    bool                _inited;

    int64_t             _frame_pts;

    YAsyncQueue<YFrame> _input_frame_queue;
    YAsyncQueue<YFrame> _output_frame_queue;

    // FFmpeg
    AVCodecContext*     _input_codec_context;
    AVCodecContext*     _output_codec_context;
    SwrContext*         _resampler_context;
    AVAudioFifo*        _audio_fifo;

    // Media parameters
    //

};
