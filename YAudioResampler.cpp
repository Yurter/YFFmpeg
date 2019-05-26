#include "YAudioResampler.h"
#include <iostream>

YAudioResampler::YAudioResampler() :
    _inited(false),
    _input_codec_context(nullptr),
    _output_codec_context(nullptr),
    _resampler_context(nullptr),
    _audio_fifo(nullptr)
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
        return false;
    }
    if (input_codec_context == nullptr) {
        std::cerr << "[YAudioResampler] input_codec_context is null" << std::endl;
        return false;
    }
    if (output_codec_context == nullptr) {
        std::cerr << "[YAudioResampler] output_codec_context is null" << std::endl;
        return false;
    }

    _input_codec_context = input_codec_context;
    _output_codec_context = output_codec_context;

    if (!initFifo()) { return false; }

    _resampler_context = swr_alloc_set_opts(
        nullptr,
        av_get_default_channel_layout(_output_codec_context->channels),
        _output_codec_context->sample_fmt,
        _output_codec_context->sample_rate,
        av_get_default_channel_layout(_input_codec_context->channels),
        _input_codec_context->sample_fmt,
        _input_codec_context->sample_rate,
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

    _inited = true;

    std::cout << "[YAudioResampler] Inited" << std::endl;
    return true;
}

bool YAudioResampler::resample(AVFrame **frame)
{
    if (!_inited) {
        std::cerr << "[YAudioResampler] Not ineted" << std::endl;
        return false;
    }






    uint64_t src_ch_layout = _input_codec_context->channel_layout;
    uint64_t dst_ch_layout = _output_codec_context->channel_layout;
    int src_rate = _input_codec_context->sample_rate;
    int dst_rate = _output_codec_context->sample_rate;
    uint8_t **src_data = (*frame)->data;
    uint8_t **dst_data = nullptr;
    int src_nb_channels = _input_codec_context->channels;
    int dst_nb_channels = _output_codec_context->channels;
    int src_linesize = (*frame)->linesize[0];
    int dst_linesize;
    int src_nb_samples = (*frame)->nb_samples;
    int dst_nb_samples;
    int max_dst_nb_samples;
    AVSampleFormat src_sample_fmt = _input_codec_context->sample_fmt;
    AVSampleFormat dst_sample_fmt = _output_codec_context->sample_fmt;
    int dst_bufsize;
    SwrContext *swr_ctx = _resampler_context;
    double t;
    int ret;










    //////////

    src_nb_channels = av_get_channel_layout_nb_channels(src_ch_layout);
    ret = av_samples_alloc_array_and_samples(&src_data, &src_linesize, src_nb_channels,
                                             src_nb_samples, src_sample_fmt, 0);
    if (ret < 0) {
        fprintf(stderr, "Could not allocate source samples\n");
        return false;
    }

    /* compute the number of converted samples: buffering is avoided
     * ensuring that the output buffer will contain at least all the
     * converted input samples */
    max_dst_nb_samples = dst_nb_samples =
        av_rescale_rnd(src_nb_samples, dst_rate, src_rate, AV_ROUND_UP);

    /* buffer is going to be directly written to a rawaudio file, no alignment */
    dst_nb_channels = av_get_channel_layout_nb_channels(dst_ch_layout);
    ret = av_samples_alloc_array_and_samples(&dst_data, &dst_linesize, dst_nb_channels,
                                             dst_nb_samples, dst_sample_fmt, 0);
    if (ret < 0) {
        fprintf(stderr, "Could not allocate destination samples\n");
        return false;
    }


    /* compute destination number of samples */
    dst_nb_samples = av_rescale_rnd(swr_get_delay(swr_ctx, src_rate) +
                                    src_nb_samples, dst_rate, src_rate, AV_ROUND_UP);
    if (dst_nb_samples > max_dst_nb_samples) {
        av_freep(&dst_data[0]);
        ret = av_samples_alloc(dst_data, &dst_linesize, dst_nb_channels,
                               dst_nb_samples, dst_sample_fmt, 1);
        if (ret < 0)
            return false;
        max_dst_nb_samples = dst_nb_samples;
    }

    /* convert to destination format */
    ret = swr_convert(swr_ctx, dst_data, dst_nb_samples, (const uint8_t **)src_data, src_nb_samples);
    if (ret < 0) {
        fprintf(stderr, "Error while converting\n");
        return false;
    }
    dst_bufsize = av_samples_get_buffer_size(&dst_linesize, dst_nb_channels,
                                             ret, dst_sample_fmt, 1);
    if (dst_bufsize < 0) {
        fprintf(stderr, "Could not get sample buffer size\n");
        return false;
    }
    printf("t:%f in:%d out:%d\n", t, src_nb_samples, ret);
//    fwrite(dst_data[0], 1, dst_bufsize, dst_file);

//    (*frame)->data[0] = *dst_data;
//    (*frame)->data[1] = *dst_data;
//    (*frame)->data = nullptr;
    (*frame)->extended_data = dst_data;
    (*frame)->nb_samples = dst_nb_samples;
//    (*frame)->linesize[0] = dst_linesize;
//    (*frame)->data = dst_data;

    return true;
}

bool YAudioResampler::initFifo()
{
    /* Create the FIFO buffer based on the specified output sample format. */
    _audio_fifo = av_audio_fifo_alloc(_output_codec_context->sample_fmt,
                                      _output_codec_context->channels, 1);
    if (_audio_fifo == nullptr) {
        std::cerr << "[YAudioResampler] Could not allocate FIFO" << std::endl;
        return false;
    }
    return true;
}

bool YAudioResampler::initConvertedSamples(uint8_t ***converted_input_samples, int64_t frame_size)
{
    /* Allocate as many pointers as there are audio channels.
     * Each pointer will later point to the audio samples of the corresponding
     * channels (although it may be NULL for interleaved formats). */
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

bool YAudioResampler::addSamplesToFifo(uint8_t **converted_input_samples, const int frame_size)
{
    /* Make the FIFO as large as it needs to be to hold both,
     * the old and the new samples. */
    if (av_audio_fifo_realloc(_audio_fifo, av_audio_fifo_size(_audio_fifo) + frame_size) < 0) {
        std::cerr << "[YAudioResampler] Could not reallocate FIFO" << std::endl;
        return false;
    }
    /* Store the new samples in the FIFO buffer. */
    if (av_audio_fifo_write(_audio_fifo, reinterpret_cast<void**>(converted_input_samples), frame_size) < frame_size) {
        std::cerr << "[YAudioResampler] Could not write data to FIFO" << std::endl;
        return false;
    }
    return true;
}

bool YAudioResampler::initOutputFrame(AVFrame **frame, int frame_size)
{
    /* Create a new frame to store the audio samples. */
    if (!(*frame = av_frame_alloc())) {
        std::cerr << "[YAudioResampler] Could not allocate output frame" << std::endl;
        return false;
    }
    /* Set the frame's parameters, especially its size and format.
     * av_frame_get_buffer needs this to allocate memory for the
     * audio samples of the frame.
     * Default channel layouts based on the number of channels
     * are assumed for simplicity. */
    (*frame)->nb_samples     = frame_size;
    (*frame)->channel_layout = _output_codec_context->channel_layout;
    (*frame)->format         = _output_codec_context->sample_fmt;
    (*frame)->sample_rate    = _output_codec_context->sample_rate;
    /* Allocate the samples of the created frame. This call will make
     * sure that the audio frame can hold as many samples as specified. */
    if (av_frame_get_buffer(*frame, 0) < 0) {
        std::cerr << "[YAudioResampler] Could not allocate output frame samples" << std::endl;
        av_frame_free(frame);
        return false;
    }
    return true;
}
