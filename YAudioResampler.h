#pragma once

#include "ffmpeg.h"

class YAudioResampler
{

public:

    YAudioResampler();
    ~YAudioResampler();

    bool init(AVCodecContext *input_codec_context, AVCodecContext *output_codec_context);

    bool resample(AVFrame **frame);

private:

    bool initFifo();
    bool initConvertedSamples(uint8_t ***converted_input_samples, int64_t frame_size);
    bool convertSamples(const uint8_t **input_data, uint8_t **converted_data, const int frame_size);
    bool addSamplesToFifo(uint8_t **converted_input_samples, const int frame_size);
    bool initOutputFrame(AVFrame **frame, int frame_size);

protected:

    // General parameters
    bool                _inited;

    // FFmpeg
    AVCodecContext*     _input_codec_context;
    AVCodecContext*     _output_codec_context;
    SwrContext*         _resampler_context;
    AVAudioFifo*        _audio_fifo;

    // Media parameters
};
