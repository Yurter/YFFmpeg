#include "ResamplerContext.hpp"
#include <core/FFmpegException.hpp>
#include <core/Logger.hpp>
#include <core/Utils.hpp>

namespace fpp {

    ResamplerContext::ResamplerContext(IOParams parameters)
        : params { parameters } {
        init();
    }

    FrameList ResamplerContext::resample(Frame source_frame) {
        const int ret = swr_convert_frame(_resampler_context.get(), nullptr, &source_frame.raw());
        if (ret != 0) {
            throw FFmpegException { "swr_convert_frame failed", ret };
        }
        const auto audio_params = std::static_pointer_cast<const AudioParameters>(params.out);
        FrameList result;
        do {
            Frame resampled_frame = createFrame();
            const int ret = swr_convert_frame(_resampler_context.get(), &resampled_frame.raw(), nullptr);
            if (ret != 0) {
                throw FFmpegException { "swr_convert_frame failed", ret };
            }
            resampled_frame.setType(params.in->type());
            resampled_frame.setPts(source_frame.pts()); //TODO проверить необходимость ручной установки 24.01

            result.push_back(resampled_frame);
        } while (swr_get_out_samples(_resampler_context.get(), 0) >= audio_params->frameSize());

        return result;
    }

    // TODO https://stackoverflow.com/questions/12831761/how-to-resize-a-picture-using-ffmpegs-sws-scale
    void ResamplerContext::init() {
        auto in_param = std::static_pointer_cast<const AudioParameters>(params.in);
        auto out_param = std::static_pointer_cast<const AudioParameters>(params.out);
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
        return frame;
    }

} // namespace fpp
