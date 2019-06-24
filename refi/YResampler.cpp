#include "YResampler.h"
#include <iostream>

YResampler::YResampler(streams_pair audio_streams) :
    YAbstractRefi(audio_streams),
    _frame_pts(0),
    _resampler_context(nullptr)
{
    setName("YResampler");
    setSkipType(YMediaType::MEDIA_TYPE_VIDEO);
}

YResampler::~YResampler()
{
    if (_resampler_context != nullptr) {
        swr_free(&_resampler_context);
    }
}

//YCode YResampler::init(AVCodecContext *input_codec_context, AVCodecContext *output_codec_context)
YCode YResampler::init()
{
    return_if(inited(), YCode::INVALID_CALL_ORDER);
    return_if_not(_io_streams.first->inited(), YCode::INVALID_INPUT);
    return_if_not(_io_streams.second->inited(), YCode::INVALID_INPUT);

    auto in_param = dynamic_cast<YAudioStream*>(_io_streams.first)->parameters; //TODO
    auto out_param = dynamic_cast<YAudioStream*>(_io_streams.second)->parameters;

    _resampler_context = swr_alloc_set_opts(
        nullptr,
        av_get_default_channel_layout(int(out_param.channels())),
        out_param.sampleFormat(),
        int(out_param.sampleRate()),
        av_get_default_channel_layout(int(in_param.channels())),
        in_param.sampleFormat(),
        int(in_param.sampleRate()),
        0, nullptr
    );
    if (_resampler_context == nullptr) {
        log_error("Could not allocate resampler context");
        return YCode::ERR;
    }
    if (swr_init(_resampler_context) < 0) {
        log_error("Could not open resample context");
        swr_free(&_resampler_context);
        return YCode::ERR;
    }
    setInited(true);
    return YCode::OK;
}

YCode YResampler::processInputData(YFrame &input_data)
{
    return_if_not(inited(), YCode::NOT_INITED);
    if (swr_convert_frame(_resampler_context, nullptr, input_data.raw()) != 0) {
        std::cerr << "[YResampler] PUSH swr_convert_frame failed " << std::endl;
        return YCode::ERR;
    }
    do {
        AVFrame *output_frame = nullptr;
        if (!initOutputFrame(&output_frame, _io_streams.second->codecParameters()->frame_size)) {
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

        try_to(sendOutputData(output_data));
    } while (swr_get_out_samples(_resampler_context, 0) >= _io_streams.second->codecParameters()->frame_size);

    return YCode::OK;
}

bool YResampler::initOutputFrame(AVFrame **frame, int frame_size)
{
    auto in_param = dynamic_cast<YAudioStream*>(_io_streams.first)->parameters; //TODO
    auto out_param = dynamic_cast<YAudioStream*>(_io_streams.second)->parameters;
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
    (*frame)->channel_layout = out_param.chanelsLayout();
    (*frame)->format         = out_param.sampleFormat();
    (*frame)->sample_rate    = out_param.sampleFormat();
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
    auto in_param = dynamic_cast<YAudioStream*>(_io_streams.first)->parameters; //TODO
    auto out_param = dynamic_cast<YAudioStream*>(_io_streams.second)->parameters;

    if (in) {
        if (in_param.chanelsLayout() != in->channel_layout
                || in_param.sampleRate() != in->sample_rate
                || in_param.sampleFormat()  != in->format) {
            return true;
        }
    }
    if (out) {
        if (out_param.chanelsLayout() != out->channel_layout
                || out_param.sampleRate() != out->sample_rate
                || out_param.sampleFormat()  != out->format) {
            return true;
        }
    }
    return false;
}

void YResampler::stampFrame(AVFrame *frame)
{
//    frame->pts = _frame_pts * 23;
//    frame->pkt_duration = 23;
    _frame_pts++;
}

