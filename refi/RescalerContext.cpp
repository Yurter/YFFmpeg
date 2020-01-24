#include "RescalerContext.hpp"
#include <core/FFmpegException.hpp>
#include <core/Logger.hpp>
#include <core/Utils.hpp>

namespace fpp {

    RescalerContext::RescalerContext(IOParams parameters)
        : params { parameters } {
        init();
    }

    Frame RescalerContext::scale(Frame source_frame) {
        Frame rescaled_frame = createFrame();
        sws_scale(_rescaler_context.get(), source_frame.raw().data
            , source_frame.raw().linesize, 0, source_frame.raw().height
            , rescaled_frame.raw().data, rescaled_frame.raw().linesize
        );
        av_frame_copy_props(&rescaled_frame.raw(), &source_frame.raw());
        return rescaled_frame;
    }

    void RescalerContext::init() {
        auto input_params = std::dynamic_pointer_cast<VideoParameters>(params.in);
        auto output_params = std::dynamic_pointer_cast<VideoParameters>(params.out);
        _rescaler_context = SharedSwsContext {
            sws_getContext(
                int(input_params->width()), int(input_params->height()), input_params->pixelFormat()
                , int(output_params->width()), int(output_params->height()), output_params->pixelFormat()
                , SWS_BICUBIC, nullptr, nullptr, nullptr
            )
            , [](ffmpeg::SwsContext* ctx) { sws_freeContext(ctx); }
        };
        if (!_rescaler_context) {
            throw FFmpegException { "sws_getContext failed" };
        }
        log_info("Inited "
            << "from"
                << " [" << input_params->width()
                << "x"  << input_params->height()
                << ", " << utils::to_string(input_params->pixelFormat())
                << "("  << input_params->pixelFormat() << ")"
                << "] "
            << "to"
                << " [" << output_params->width()
                << "x"  << output_params->height()
                << ", " << utils::to_string(output_params->pixelFormat())
                << "("  << output_params->pixelFormat() << ")"
                << "]"
        );
    }

    Frame RescalerContext::createFrame() const {
        Frame frame;
        const auto output_params = std::static_pointer_cast<const VideoParameters>(params.out);
        frame.raw().format = output_params->pixelFormat();
        frame.raw().width  = int(output_params->width());
        frame.raw().height = int(output_params->height());
        frame.setType(output_params->type());
        const int ret = av_frame_get_buffer(&frame.raw(), 32);
        if (ret < 0) {
            throw FFmpegException { "av_frame_get_buffer failed", ret };
        }
        return frame;
    }

} // namespace fpp
