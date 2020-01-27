#include "FilterContext.hpp"
#include <fpp/core/Logger.hpp>
#include <fpp/core/FFmpegException.hpp>

namespace ffmpeg { extern "C" {
    #include <libavfilter/avfilter.h>
    #include <libavfilter/buffersink.h>
    #include <libavfilter/buffersrc.h>
    #include <libavutil/opt.h>
} } // namespace ffmpeg

namespace fpp {

    FilterContext::FilterContext(SharedParameters parameters, const std::string& filters_descr)
        : params(parameters)
        , _filters_descr(filters_descr) {
        setName("Filter");
    }

    FrameList FilterContext::filter(Frame frame) {  //TODO много параметров захардкожено 14.01
        if (av_buffersrc_add_frame_flags(_buffersrc_ctx, &frame.raw(), ffmpeg::AV_BUFFERSRC_FLAG_KEEP_REF) < 0) {
            throw FFmpegException { "av_buffersrc_add_frame_flags failed" };
        }
        FrameList filtered_frames;
        /* pull filtered frames from the filtergraph */
        while (true) {
            Frame output_frame;
            int ret = ffmpeg::av_buffersink_get_frame(_buffersink_ctx, &output_frame.raw());
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                return filtered_frames;
            if (ret < 0) {
                throw FFmpegException { "av_buffersink_get_frame failed" };
            }
            output_frame.setType(MediaType::Video);
            output_frame.raw().linesize[0] = frame.raw().linesize[0]; //TODO убрать?
            output_frame.raw().linesize[1] = frame.raw().linesize[1];
            output_frame.raw().linesize[2] = frame.raw().linesize[2];
            filtered_frames.push_back(output_frame);
        }
    }

    std::string FilterContext::description() const {
        return _filters_descr;
    }

    void FilterContext::init() {
        auto video_params = std::static_pointer_cast<const VideoParameters>(params);

        char args[512];
        int ret = 0;
        const ffmpeg::AVFilter* buffersrc = ffmpeg::avfilter_get_by_name("buffer");
        const ffmpeg::AVFilter* buffersink = ffmpeg::avfilter_get_by_name("buffersink");
        ffmpeg::AVFilterInOut* outputs = ffmpeg::avfilter_inout_alloc(); //TODO убрать голый указатель 20.01
        ffmpeg::AVFilterInOut* inputs  = ffmpeg::avfilter_inout_alloc(); //TODO убрать голый указатель 20.01
        enum ffmpeg::AVPixelFormat pix_fmts[] = { video_params->pixelFormat(), ffmpeg::AV_PIX_FMT_NONE };

        _filter_graph = ffmpeg::avfilter_graph_alloc();
        if (!outputs || !inputs || !_filter_graph) {
            ret = AVERROR(ENOMEM);
            throw FFmpegException { "avfilter_graph_alloc failed", ret };
        }

        /* buffer video source: the decoded frames from the decoder will be inserted here. */
        snprintf(args, sizeof(args),
                "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
                int(video_params->width()), int(video_params->height()), video_params->pixelFormat(),
                video_params->timeBase().num, video_params->timeBase().den,
                video_params->aspectRatio().num, video_params->aspectRatio().den);

        ret = avfilter_graph_create_filter(&_buffersrc_ctx, buffersrc, "in",
                                           args, nullptr, _filter_graph);
        if (ret < 0) {
            throw FFmpegException { "avfilter_graph_create_filter (in) failed", ret };
        }

        log_info("Filter in inited with args: " << args);

        /* buffer video sink: to terminate the filter chain. */
        ret = avfilter_graph_create_filter(&_buffersink_ctx, buffersink, "out",
                                           nullptr, nullptr, _filter_graph);
        if (ret < 0) {
            throw FFmpegException { "avfilter_graph_create_filter (out) failed", ret };
        }

        snprintf(args, sizeof(args), "NULL");
        log_info("Filter out inited with args: " << args);

        ret = av_opt_set_int_list(_buffersink_ctx, "pix_fmts", pix_fmts,
                                  ffmpeg::AV_PIX_FMT_NONE, AV_OPT_SEARCH_CHILDREN);
        if (ret < 0) {
            throw FFmpegException { "av_opt_set_int_list failed", ret };
        }

        /*
         * Set the endpoints for the filter graph. The filter_graph will
         * be linked to the graph described by filters_descr.
         */

        /*
         * The buffer source output must be connected to the input pad of
         * the first filter described by filters_descr; since the first
         * filter input label is not specified, it is set to "in" by
         * default.
         */
        outputs->name       = ffmpeg::av_strdup("in");
        outputs->filter_ctx = _buffersrc_ctx;
        outputs->pad_idx    = 0;
        outputs->next       = nullptr;

        /*
         * The buffer sink input must be connected to the output pad of
         * the last filter described by filters_descr; since the last
         * filter output label is not specified, it is set to "out" by
         * default.
         */
        inputs->name       = ffmpeg::av_strdup("out");
        inputs->filter_ctx = _buffersink_ctx;
        inputs->pad_idx    = 0;
        inputs->next       = nullptr;

        if ((ret = avfilter_graph_parse_ptr(_filter_graph, _filters_descr.c_str()
                                            , &inputs, &outputs, nullptr)) < 0) {
            throw FFmpegException { "avfilter_graph_parse_ptr failed", ret };
        }

        log_info("Filter description: " << _filters_descr);

        if ((ret = avfilter_graph_config(_filter_graph, nullptr)) < 0) {
            throw FFmpegException { "avfilter_graph_config failed", ret };
        }

        avfilter_inout_free(&inputs);
        avfilter_inout_free(&outputs);
    }

} // namespace fpp
