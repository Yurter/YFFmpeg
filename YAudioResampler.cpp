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

void YAudioResampler::push(YFrame frame)
{
    _input_frame_queue.push(frame);
}

bool YAudioResampler::pop(YFrame &frame)
{
    return _output_frame_queue.pop(frame);
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

YCode YAudioResampler::run()
{
    if (!_inited) {
        std::cerr << "[YAudioResampler] Not inited" << std::endl;
        return YCode::ERR;
    }
    YFrame input_frame;
    if (!_input_frame_queue.pop(input_frame)) {
        return YCode::AGAIN;
    }
//    YFrame output_frame;
//    AVFrame *out_frame = output_frame.raw();
//    AVFrame *out_frame = av_frame_alloc();
//    YFrame output_frame;
    AVFrame* raw_frame = nullptr;
    if (!initOutputFrame(&raw_frame, _output_codec_context->frame_size)) {
        std::cerr << "[YAudioResampler] initOutputFrame failed" << std::endl;
        return YCode::ERR;
    }
    YFrame output_frame(raw_frame);
    output_frame.setType(MEDIA_TYPE_AUDIO);

    if (configChanged(input_frame.raw(), output_frame.raw())) {
        std::cerr << "[YAudioResampler] configChanged" << std::endl;
        return YCode::ERR;
    }

    int ret = 0;
    if ((ret = swr_convert_frame(_resampler_context, output_frame.raw(), input_frame.raw())) != 0) {
        std::cerr << "[YAudioResampler] swr_convert_frame failed " << ret << std::endl;
        return YCode::ERR;
    }

//    stampFrame(output_frame); // output_frame->pts = AV_NOPTS_VALUE;

//    (*frame) = output_frame;

    _output_frame_queue.push(output_frame);

    std::cout << "[YAudioResampler] swr_convert_frame success " << std::endl;

    return YCode::OK;
}

//bool YAudioResampler::resample(AVFrame **frame)
//{
//    if (!_inited) {
//        std::cerr << "[YAudioResampler] Not inited" << std::endl;
//        return false;
//    }

//    AVFrame *output_frame = nullptr;
//    if (!initOutputFrame(&output_frame, _output_codec_context->frame_size)) {
//        std::cerr << "[YAudioResampler] initOutputFrame failed" << std::endl;
//        return false;
//    }

//    if (configChanged(*frame, output_frame)) {
//        std::cerr << "[YAudioResampler] configChanged" << std::endl;
//        return false;
//    }

//    int ret = 0;
//    if ((ret = swr_convert_frame(_resampler_context, output_frame, *frame)) != 0) {
//        std::cerr << "[YAudioResampler] swr_convert_frame failed " << ret << std::endl;
//        return false;
//    }

////    stampFrame(output_frame); // output_frame->pts = AV_NOPTS_VALUE;

//    (*frame) = output_frame;

//    return true;
//}

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

bool YAudioResampler::configChanged(const AVFrame *in, const AVFrame *out)
{
    if (in) {
        if (_input_codec_context->channel_layout != in->channel_layout
                || _input_codec_context->sample_rate != in->sample_rate
                || _input_codec_context->sample_fmt  != in->format) {
            return true;
        }
    }

    if (out) {
        if (_output_codec_context->channel_layout != out->channel_layout ||
            _output_codec_context->sample_rate != out->sample_rate ||
            _output_codec_context->sample_fmt  != out->format) {
            return true;
        }
    }

    return false;
}

void YAudioResampler::stampFrame(AVFrame *frame)
{
    //frame->pts = _frame_pts;
    _frame_pts++;
}
