#include "YResampler.h"
#include <iostream>

YResampler::YResampler() :
    _inited(false),
    _frame_pts(0),
    _input_codec_context(nullptr),
    _output_codec_context(nullptr),
    _resampler_context(nullptr),
    _audio_fifo(nullptr)
{
    //
}

YResampler::~YResampler()
{
    if (_resampler_context != nullptr) {
        swr_free(&_resampler_context);
    }
}

bool YResampler::init(AVCodecContext *input_codec_context, AVCodecContext *output_codec_context)
{
    if (_inited) {
        std::cerr << "[YResampler] Already inited" << std::endl;
        return false;
    }
    if (input_codec_context == nullptr) {
        std::cerr << "[YResampler] input_codec_context is null" << std::endl;
        return false;
    }
    if (output_codec_context == nullptr) {
        std::cerr << "[YResampler] output_codec_context is null" << std::endl;
        return false;
    }

    _input_codec_context = input_codec_context;
    _output_codec_context = output_codec_context;

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
        std::cerr << "[YResampler] Could not allocate resampler context" << std::endl;
        return false;
    }

    if (swr_init(_resampler_context) < 0) {
        std::cerr << "[YResampler] Could not open resample context" << std::endl;
        swr_free(&_resampler_context);
        return false;
    }

    _inited = true;

    std::cout << "[YResampler] Inited" << std::endl;
    return true;
}

YCode YResampler::processInputData(YFrame &input_data)
{
    if (!_inited) {
        return YCode::NOT_INITED;
    }
    if (swr_convert_frame(_resampler_context, nullptr, input_data.raw()) != 0) {
        std::cerr << "[YResampler] PUSH swr_convert_frame failed " << std::endl;
        return YCode::ERR;
    }
    do {
        AVFrame *output_frame = nullptr;
        if (!initOutputFrame(&output_frame, _output_codec_context->frame_size)) {
            std::cerr << "[YResampler] initOutputFrame failed" << std::endl;
            return YCode::ERR;
        }
        if (configChanged(input_data.raw(), output_frame)) {
            std::cerr << "[YResampler] configChanged" << std::endl;
            return YCode::ERR;
        }
        if (swr_convert_frame(_resampler_context, output_frame, nullptr) != 0) {
            std::cerr << "[YResampler] swr_convert_frame failed " << std::endl;
            return YCode::ERR;
        }
        YFrame output_data(output_frame);
        output_data.setType(MEDIA_TYPE_AUDIO);
        sendOutputData(output_data);
    } while (swr_get_out_samples(_resampler_context, 0) >= _output_codec_context->frame_size);

    return YCode::OK;
}

bool YResampler::initOutputFrame(AVFrame **frame, int frame_size)
{
    /* Create a new frame to store the audio samples. */
    if (!(*frame = av_frame_alloc())) {
        std::cerr << "[YResampler] Could not allocate output frame" << std::endl;
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
        std::cerr << "[YResampler] Could not allocate output frame samples" << std::endl;
        av_frame_free(frame);
        return false;
    }
    return true;
}

//bool YResampler::configChanged(const AVFrame *in, const AVFrame *out)
bool YResampler::configChanged(AVFrame *in, AVFrame *out)
{
    if (in) {
        if (_input_codec_context->channel_layout != in->channel_layout
                || _input_codec_context->sample_rate != in->sample_rate
                || _input_codec_context->sample_fmt  != in->format) {
            return true;
        }
    }
    if (out) {
        if (_output_codec_context->channel_layout != out->channel_layout
                || _output_codec_context->sample_rate != out->sample_rate
                || _output_codec_context->sample_fmt  != out->format) {
            return true;
        }
    }
    return false;
}

void YResampler::stampFrame(AVFrame *frame)
{
    //frame->pts = _frame_pts;
    _frame_pts++;
}
