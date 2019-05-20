#include "YAudioResampler.h"

YAudioResampler::YAudioResampler()
{
    //
}

YAudioResampler::~YAudioResampler()
{
    //
}

bool YAudioResampler::init(AVCodecContext *input_codec_context, AVCodecContext *output_codec_context)
{
    int error;
    _resample_context = swr_alloc_set_opts(
                nullptr,
                av_get_default_channel_layout(output_codec_context->channels),
                output_codec_context->sample_fmt,
                output_codec_context->sample_rate,
                av_get_default_channel_layout(input_codec_context->channels),
                input_codec_context->sample_fmt,
                input_codec_context->sample_rate,
                0, nullptr
    );

//    struct SwrContext *swr_alloc(void);
//    av_opt_set_channel_layout(swr, "in_channel_layout", AV_CH_LAYOUT_STEREO, 0);
//    av_opt_set_channel_layout(swr, "out_channel_layout", AV_CH_LAYOUT_STEREO, 0);
//    av_opt_set_int(swr, "in_sample_rate",  44100, 0);
//    av_opt_set_int(swr, "out_sample_rate", 22050, 0);
//    av_opt_set_sample_fmt(swr, "in_sample_fmt", AV_SAMPLE_FMT_FLT, 0);
//    av_opt_set_sample_fmt(swr, "out_sample_fmt", AV_SAMPLE_FMT_FLT, 0);
//    swr_init(swr);

    if (!_resample_context) {
        fprintf(stderr, "Could not allocate resample context\n");
        return false;
    }

    if ((error = swr_init(_resample_context)) < 0) {
        fprintf(stderr, "Could not open resample context\n");
        swr_free(&_resample_context);
        return false;
    }

    return true;
}

bool YAudioResampler::resample(AVFrame *frame)
{
    int error;
    /* Convert the samples using the resampler. */
//    if ((error = swr_convert(_resample_context, frame->extended_data, frame->linesize[0], const_cast<const uint8_t**>(frame->extended_data), frame->linesize[0])) < 0) {
//        fprintf(stderr, "Could not convert input samples (error '')\n");
//        return error;
//    }

    // resample frames
    double* buffer;
    av_samples_alloc((uint8_t**) &buffer, nullptr, 1, frame->nb_samples, AV_SAMPLE_FMT_DBL, 0);
    int frame_count = swr_convert(_resample_context, (uint8_t**) &buffer, frame->nb_samples, (const uint8_t**) frame->data, frame->nb_samples);
    // append resampled frames to data
    frame->data[0] = (uint8_t*)buffer;


//    uint8_t **dst_data = nullptr;
//    int dst_nb_channels = 2;
//    int dst_linesize;
//    int dst_nb_samples = frame->nb_samples;
//    enum AVSampleFormat dst_sample_fmt = AV_SAMPLE_FMT_S16;
//    error = av_samples_alloc_array_and_samples(&dst_data, &dst_linesize, dst_nb_channels, dst_nb_samples, dst_sample_fmt, 0);


    return true;
}
