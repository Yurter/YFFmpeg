#include "Resampler.hpp"

namespace fpp {

    Resampler::Resampler(IOParams params)
        : params(params)
        , _resampler_context(nullptr) {
        setName("Resampler");
    }

    Resampler::~Resampler() {
        if (_resampler_context != nullptr) {
//            swr_free(&_resampler_context);
        }
    }

    Code Resampler::init() {
        return_if(inited(), Code::INVALID_CALL_ORDER);
    //    return_if_not(_io_streams.first->inited(), Code::INVALID_INPUT); //TODO
    //    return_if_not(_io_streams.second->inited(), Code::INVALID_INPUT);

    //    auto in_param = dynamic_cast<AudioStream*>(_io_streams.first)->parameters; //TODO
    //    auto out_param = dynamic_cast<AudioStream*>(_io_streams.second)->parameters;
        auto in_param = dynamic_cast<const AudioParameters * const>(params.in.get());
        auto out_param = dynamic_cast<const AudioParameters * const>(params.out.get());

        _resampler_context = SharedSwrContext {
                swr_alloc_set_opts(
                            nullptr,
                            av_get_default_channel_layout(int(out_param->channels())),
                            out_param->sampleFormat(),
                            int(out_param->sampleRate()),
                            av_get_default_channel_layout(int(in_param->channels())),
                            in_param->sampleFormat(),
                            int(in_param->sampleRate()),
                            0, nullptr
                        )
                , [](SwrContext*& ctx) { swr_free(&ctx); }
        };

//        _resampler_context = swr_alloc_set_opts(
//            nullptr,
//            av_get_default_channel_layout(int(out_param->channels())),
//            out_param->sampleFormat(),
//            int(out_param->sampleRate()),
//            av_get_default_channel_layout(int(in_param->channels())),
//            in_param->sampleFormat(),
//            int(in_param->sampleRate()),
//            0, nullptr
//        );
        if (_resampler_context == nullptr) {
            log_error("Could not allocate resampler context");
            return Code::ERR;
        }
        if (swr_init(_resampler_context.get()) < 0) {
            log_error("Could not open resample context");
//            swr_free(&_resampler_context); ??TODO !!!
            return Code::ERR;
        }
        setInited(true);
        return Code::OK;
    }

    Code Resampler::open() { //TODO перенести пустую реализацию в базовое определение метода
        setOpened(true);
        return Code::OK;
    }

    Code Resampler::close() { //TODO перенести пустую реализацию в базовое определение метода
        setOpened(false);
        return Code::OK;
    }

    Code Resampler::processInputData(Frame input_data) {
        if (swr_convert_frame(_resampler_context.get(), nullptr, &input_data.raw()) != 0) {
            log_error("swr_convert_frame failed");
            return Code::ERR;
        }
        auto audio_params = static_cast<const AudioParameters * const>(params.out.get());
        do {
            Frame resampled_frame; // TODO frameSize брать из контекста энкодера
            if (!initOutputFrame(resampled_frame, audio_params->frameSize())) {
                log_error("initOutputFrame failed");
                return Code::ERR;
            }
            if (configChanged(&input_data.raw(), &resampled_frame.raw())) {
                log_error("configChanged failed");
                return Code::ERR;
            }
            if (swr_convert_frame(_resampler_context.get(), &resampled_frame.raw(), nullptr) != 0) {
                log_error("swr_convert_frame failed");
                return Code::ERR;
            }
            resampled_frame.setType(params.in->type());
//            resampled_frame.setTimeBase(params.in->timeBase());
            resampled_frame.setPts(input_data.pts());

//            log_warning("RESAMPLED: " << resampled_frame);
            try_to(sendOutputData(resampled_frame));
        } while (swr_get_out_samples(_resampler_context.get(), 0) >= audio_params->frameSize());

        return Code::OK;
    }

//    bool Resampler::initOutputFrame(AVFrame** frame, int frame_size) {
//        frame_size = 1024; //TODO critical!
//        auto in_param = dynamic_cast<const AudioParameters * const>(params.in);
//        auto out_param = dynamic_cast<const AudioParameters * const>(params.out);
//        /* Create a new frame to store the audio samples. */
//        if (!(*frame = av_frame_alloc())) {
//            log_error("Could not allocate output frame");
//            return false;
//        }
//        /* Set the frame's parameters, especially its size and format.
//         * av_frame_get_buffer needs this to allocate memory for the
//         * audio samples of the frame.
//         * Default channel layouts based on the number of channels
//         * are assumed for simplicity. */
//        (*frame)->nb_samples     = frame_size;
//        (*frame)->channel_layout = out_param->channelLayout();
//        (*frame)->format         = out_param->sampleFormat();
//        (*frame)->sample_rate    = int(out_param->sampleRate());
//        /* Allocate the samples of the created frame. This call will make
//         * sure that the audio frame can hold as many samples as specified. */
//        if (av_frame_get_buffer(*frame, 0) < 0) {
//            log_error("Could not allocate output frame samples");
//            av_frame_free(frame);
//            return false;
//        }
//        return true;
//    }
    // ПЕРЕДЕЛКИ С УКАЗАТЕЛЯ
    bool Resampler::initOutputFrame(Frame& frame, int64_t frame_size) {
        frame_size = 1024; //TODO critical!
        auto in_param = dynamic_cast<const AudioParameters * const>(params.in.get());
        auto out_param = dynamic_cast<const AudioParameters * const>(params.out.get());
        /* Set the frame's parameters, especially its size and format.
         * av_frame_get_buffer needs this to allocate memory for the
         * audio samples of the frame.
         * Default channel layouts based on the number of channels
         * are assumed for simplicity. */
        frame.raw().nb_samples     = int(frame_size);
        frame.raw().channel_layout = out_param->channelLayout();
        frame.raw().format         = out_param->sampleFormat();
        frame.raw().sample_rate    = int(out_param->sampleRate());
        /* Allocate the samples of the created frame. This call will make
         * sure that the audio frame can hold as many samples as specified. */
        if (av_frame_get_buffer(&frame.raw(), 0) < 0) {
            log_error("Could not allocate output frame samples");
            return false;
        }
        return true;
    }

    //bool Resampler::configChanged(const AVFrame *in, const AVFrame *out)
    bool Resampler::configChanged(AVFrame* in, AVFrame* out) {
        auto in_param = dynamic_cast<const AudioParameters * const>(params.in.get()); //TODO убрать динамичный каст
        auto out_param = dynamic_cast<const AudioParameters * const>(params.out.get());

        if (in) {
            if (in_param->channelLayout() != in->channel_layout
                    || in_param->sampleRate() != in->sample_rate
                    || in_param->sampleFormat()  != in->format) {
                log_error("in_param: " << in_param->toString());
                log_error("in->channel_layout = " << in->channel_layout << " "
                          << "in->sample_rate = " << in->sample_rate << " "
                          << "in->format " << in->format);
                return true;
            }
        }
        if (out) {
            if (out_param->channelLayout() != out->channel_layout
                    || out_param->sampleRate() != out->sample_rate
                    || out_param->sampleFormat()  != out->format) {
                log_error("out_param: " << out_param->toString());
                log_error("out->channel_layout = " << out->channel_layout << " "
                          << "out->sample_rate = " << out->sample_rate << " "
                          << "out->format " << out->format);
                return true;
            }
        }
        return false;
    }

} // namespace fpp
