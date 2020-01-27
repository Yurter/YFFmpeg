#include "ResamplerContext.hpp"
#include <fpp/core/FFmpegException.hpp>
#include <fpp/core/Logger.hpp>
#include <fpp/core/Utils.hpp>

namespace fpp {

    ResamplerContext::ResamplerContext(IOParams parameters)
        : params { parameters } {
        init();
    }

    FrameList ResamplerContext::resample(Frame source_frame) {
        if (swr_convert_frame(_resampler_context.get(), nullptr, &source_frame.raw()) != 0) {
            throw FFmpegException { "swr_convert_frame failed" };
        }
        const auto audio_params = std::static_pointer_cast<const AudioParameters>(params.out);
        FrameList resampled_frames;
        while (swr_get_out_samples(_resampler_context.get(), 0) >= audio_params->frameSize()) { // TODO сравнить AVERROR(EAGAIN) и swr_get_out_samples,
            Frame output_frame = createFrame();                                                 // заменить на while (true) 27.01
            int ret = ffmpeg::swr_convert_frame(_resampler_context.get(), &output_frame.raw(), nullptr);
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                return resampled_frames;
            if (ret < 0) {
                throw FFmpegException { "swr_convert_frame failed" };
            }
            output_frame.setPts(source_frame.pts()); //TODO проверить необходимость ручной установки 24.01
            resampled_frames.push_back(output_frame);
        }
        return resampled_frames;
    }

    // TODO https://stackoverflow.com/questions/12831761/how-to-resize-a-picture-using-ffmpegs-sws-scale
    void ResamplerContext::init() {
        const auto in_param = std::static_pointer_cast<const AudioParameters>(params.in);
        const auto out_param = std::static_pointer_cast<const AudioParameters>(params.out);
        _resampler_context = SharedSwrContext {
            swr_alloc_set_opts(
                nullptr
                , ffmpeg::av_get_default_channel_layout(int(out_param->channels())) //TODO юзать метод setChannelLayout() 24.01
                , out_param->sampleFormat()
                , int(out_param->sampleRate())
                , ffmpeg::av_get_default_channel_layout(int(in_param->channels()))  //TODO юзать метод setChannelLayout() 24.01
                , in_param->sampleFormat()
                , int(in_param->sampleRate())
                , 0
                , nullptr
            )
            , [](ffmpeg::SwrContext* ctx) { swr_free(&ctx); }
        };
        if (!_resampler_context) {
            throw FFmpegException { "swr_alloc_set_opts failed" };
        }
        int ret = swr_init(_resampler_context.get());
        if (ret < 0) {
            throw FFmpegException { "swr_init failed", ret };
        }
        log_info("Inited "
            << "from "
                << "ch_layout " << in_param->channelLayout()
                << ", sample_rate " << in_param->sampleFormat()
                << ", " << in_param->sampleFormat()
                << "] "
            << "to "
                 << "ch_layout " << out_param->channelLayout()
                 << ", sample_rate " << out_param->sampleFormat()
                 << ", " << out_param->sampleFormat()
                 << "] "
        );
    }

    Frame ResamplerContext::createFrame() const {
        Frame frame;
        const auto out_param = std::static_pointer_cast<const AudioParameters>(params.out);
        /* Set the frame's parameters, especially its size and format.
         * av_frame_get_buffer needs this to allocate memory for the
         * audio samples of the frame.
         * Default channel layouts based on the number of channels
         * are assumed for simplicity. */
        frame.raw().nb_samples     = int(out_param->frameSize());
        frame.raw().channel_layout = out_param->channelLayout();
        frame.raw().format         = out_param->sampleFormat();
        frame.raw().sample_rate    = int(out_param->sampleRate());
        /* Allocate the samples of the created frame. This call will make
         * sure that the audio frame can hold as many samples as specified. */
        const int ret = av_frame_get_buffer(&frame.raw(), 0);
        if (ret < 0) {
            throw FFmpegException { "av_frame_get_buffer failed", ret };
        }
        frame.setType(out_param->type());
        return frame;
    }

} // namespace fpp
