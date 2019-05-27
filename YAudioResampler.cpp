#include "YAudioResampler.h"
#include <iostream>

YAudioResampler::YAudioResampler() :
    _inited(false),
    _frame_pts(0),
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

    AVFrame *output_frame = nullptr;
    if (!initOutputFrame(&output_frame, _output_codec_context->frame_size)) {
        std::cerr << "[YAudioResampler] initOutputFrame failed" << std::endl;
        return false;
    }

    int ret = 0;
    if ((ret = swr_convert_frame(_resampler_context, output_frame, *frame)) != 0) {
        std::cerr << "[YAudioResampler] swr_convert_frame failed " << ret << std::endl;
        return false;
    }

    stampFrame(output_frame);

    (*frame) = output_frame;

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

void YAudioResampler::stampFrame(AVFrame *frame)
{
    frame->pts = _frame_pts;
    _frame_pts++;
}
