#include "YAudioResampler.h"
#include <iostream>

YAudioResampler::YAudioResampler() :
    _inited(false),
    _input_codec_context(nullptr),
    _output_codec_context(nullptr),
    _resampler_context(nullptr)
{
    //
}

YAudioResampler::~YAudioResampler()
{
    if (_resampler_context != nullptr) {
        swr_free(&_resampler_context);
    }
}

bool YAudioResampler::init(AVCodecContext *input_codec_context, AVCodecContext *output_codec_context)
{
    if (_inited) {
        std::cerr << "[YAudioResampler] Already inited" << std::endl;
    }

    _resampler_context = swr_alloc_set_opts(
        nullptr,
        av_get_default_channel_layout(output_codec_context->channels),
        output_codec_context->sample_fmt,
        output_codec_context->sample_rate,
        av_get_default_channel_layout(input_codec_context->channels),
        input_codec_context->sample_fmt,
        input_codec_context->sample_rate,
        0, nullptr
    );

    if (_resampler_context == nullptr) {
        std::cerr << "[YAudioResampler] Could not allocate resampler context" << std::endl;
        return false;
    }

    if (swr_init(_resampler_context) < 0) {
        std::cerr << "[YAudioResampler] Could not open resample context" << std::endl;
        swr_free(&_resampler_context);
        return false;
    }

    _input_codec_context = input_codec_context;
    _output_codec_context = output_codec_context;
    _inited = true;

    std::cout << "[YAudioResampler] Inited" << std::endl;
    return true;
}

bool YAudioResampler::resample(AVFrame *frame)
{
    if (!_inited) {
        std::cout << "[YAudioResampler] Not ineted" << std::endl;
        return false;
    }
    uint8_t **converted_input_samples = nullptr;
    /* Initialize the temporary storage for the converted input samples. */
    if (!initConvertedSamples(&converted_input_samples, frame->nb_samples)) {
        std::cerr << "[YAudioResampler] initConvertedSamples failed" << std::endl;
        return false;
    }
    /* Convert the input samples to the desired output sample format.
     * This requires a temporary storage provided by converted_input_samples. */
    if (!convertSamples(const_cast<const uint8_t**>(frame->extended_data),
                        converted_input_samples, frame->nb_samples)) {
        std::cerr << "[YAudioResampler] convertSamples failed" << std::endl;
        return false;
    }
    // converted_input_samples, frame->nb_samples
    frame->extended_data = converted_input_samples;
    return true;







    /*----------------------------------------------------------------------------*/
//    int error;
//    /* Convert the samples using the resampler. */
////    if ((error = swr_convert(_resample_context, frame->extended_data, frame->linesize[0], const_cast<const uint8_t**>(frame->extended_data), frame->linesize[0])) < 0) {
////        fprintf(stderr, "Could not convert input samples (error '')\n");
////        return error;
////    }

//    // resample frames
//    double* buffer;
//    av_samples_alloc((uint8_t**) &buffer, nullptr, 1, frame->nb_samples, AV_SAMPLE_FMT_DBL, 0);
//    int frame_count = swr_convert(_resample_context, (uint8_t**) &buffer, frame->nb_samples, (const uint8_t**) frame->data, frame->nb_samples);
//    // append resampled frames to data
//    frame->data[0] = (uint8_t*)buffer;


////    uint8_t **dst_data = nullptr;
////    int dst_nb_channels = 2;
////    int dst_linesize;
////    int dst_nb_samples = frame->nb_samples;
////    enum AVSampleFormat dst_sample_fmt = AV_SAMPLE_FMT_S16;
////    error = av_samples_alloc_array_and_samples(&dst_data, &dst_linesize, dst_nb_channels, dst_nb_samples, dst_sample_fmt, 0);


    //    return true;
}

bool YAudioResampler::initConvertedSamples(uint8_t ***converted_input_samples, int64_t frame_size)
{
    /* Allocate as many pointers as there are audio channels.
     * Each pointer will later point to the audio samples of the corresponding
     * channels (although it may be NULL for interleaved formats).
     */
    *converted_input_samples = reinterpret_cast<uint8_t**>(
        calloc(static_cast<size_t>(_output_codec_context->channels),
                sizeof(**converted_input_samples))
        );

    if (*converted_input_samples == nullptr) {
        std::cerr << "[YAudioResampler] Could not allocate converted input sample pointers" << std::endl;
        return false;
    }

    /* Allocate memory for the samples of all channels in one consecutive
     * block for convenience. */
    if (av_samples_alloc(*converted_input_samples, nullptr,
                          _output_codec_context->channels,
                          static_cast<int>(frame_size),
                          _output_codec_context->sample_fmt, 0) < 0) {
        std::cerr << "[YAudioResampler] Could not allocate converted input samples" << std::endl;
        av_freep(&(*converted_input_samples)[0]);
        free(*converted_input_samples);
        return false;
    }
    return true;
}

bool YAudioResampler::convertSamples(const uint8_t **input_data, uint8_t **converted_data, const int frame_size)
{
    /* Convert the samples using the resampler. */
    if (swr_convert(_resampler_context,
                     converted_data, frame_size,        // output
                     input_data    , frame_size) < 0) { // input
        std::cerr << "[YAudioResampler] Could not convert input samples" << std::endl;
        return false;
    }
    return true;
}
