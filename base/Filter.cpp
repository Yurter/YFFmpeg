#include "Filter.hpp"

namespace fpp {

    Filter::Filter(StreamPair io_streams, std::string filters_descr) :
        Refi(io_streams)
      , _filters_descr(filters_descr)
    {
        setName("Filter");
    }

    Filter::~Filter() {
        // TODO
    }

    Code Filter::init() {
        VideoParameters* out_params = dynamic_cast<VideoParameters*>(_io_streams.second->parameters);

        char args[512];
        int ret = 0;
        const AVFilter *buffersrc  = avfilter_get_by_name("buffer");
        const AVFilter *buffersink = avfilter_get_by_name("buffersink");
        AVFilterInOut *outputs = avfilter_inout_alloc();
        AVFilterInOut *inputs  = avfilter_inout_alloc();
        AVRational time_base = out_params->timeBase();
        enum AVPixelFormat pix_fmts[] = { AV_PIX_FMT_GRAY8, AV_PIX_FMT_NONE };

        _filter_graph = avfilter_graph_alloc();
        if (!outputs || !inputs || !_filter_graph) {
            ret = AVERROR(ENOMEM);
            return Code::FFMPEG_ERROR;
        }

        /* buffer video source: the decoded frames from the decoder will be inserted here. */
        snprintf(args, sizeof(args),
                "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
                int(out_params->width()), int(out_params->height()), out_params->pixelFormat(),
                time_base.num, time_base.den,
                out_params->aspectRatio().num, out_params->aspectRatio().den);

        log_debug("args: " << args);

        ret = avfilter_graph_create_filter(&_buffersrc_ctx, buffersrc, "in",
                                           args, nullptr, _filter_graph);
        if (ret < 0) {
            av_log(nullptr, AV_LOG_ERROR, "Cannot create buffer source\n");
            return Code::FFMPEG_ERROR;
        }

        /* buffer video sink: to terminate the filter chain. */
        ret = avfilter_graph_create_filter(&_buffersink_ctx, buffersink, "out",
                                           nullptr, nullptr, _filter_graph);
        if (ret < 0) {
            av_log(nullptr, AV_LOG_ERROR, "Cannot create buffer sink\n");
            return Code::FFMPEG_ERROR;
        }

        ret = av_opt_set_int_list(_buffersink_ctx, "pix_fmts", pix_fmts,
                                  AV_PIX_FMT_NONE, AV_OPT_SEARCH_CHILDREN);
        if (ret < 0) {
            av_log(nullptr, AV_LOG_ERROR, "Cannot set output pixel format\n");
            return Code::FFMPEG_ERROR;
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
        outputs->name       = av_strdup("in");
        outputs->filter_ctx = _buffersrc_ctx;
        outputs->pad_idx    = 0;
        outputs->next       = nullptr;

        /*
         * The buffer sink input must be connected to the output pad of
         * the last filter described by filters_descr; since the last
         * filter output label is not specified, it is set to "out" by
         * default.
         */
        inputs->name       = av_strdup("out");
        inputs->filter_ctx = _buffersink_ctx;
        inputs->pad_idx    = 0;
        inputs->next       = nullptr;

        if ((ret = avfilter_graph_parse_ptr(_filter_graph, _filters_descr.c_str(),
                                        &inputs, &outputs, nullptr)) < 0)
            return Code::FFMPEG_ERROR;

        if ((ret = avfilter_graph_config(_filter_graph, nullptr)) < 0)
            return Code::FFMPEG_ERROR;

    end:
        avfilter_inout_free(&inputs);
        avfilter_inout_free(&outputs);

        //return ret;
        return Code::OK;
    }

    Code Filter::processInputData(Frame& input_data) {
        if (av_buffersrc_add_frame_flags(_buffersrc_ctx, input_data.raw(), AV_BUFFERSRC_FLAG_KEEP_REF) < 0) {
            log_error("Error while feeding the filtergraph: " << input_data);
            return Code::FFMPEG_ERROR;
        }
        /* pull filtered frames from the filtergraph */
        while (1) {
            AVFrame* filt_frame = av_frame_alloc();
            int ret = av_buffersink_get_frame(_buffersink_ctx, filt_frame);
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                return Code::AGAIN;
            if (ret < 0)
                return Code::FFMPEG_ERROR;
            Frame output_data(filt_frame);
            output_data.setType(MEDIA_TYPE_VIDEO);
            output_data.raw()->linesize[0] = input_data.raw()->linesize[0];
            output_data.raw()->linesize[1] = input_data.raw()->linesize[1];
            output_data.raw()->linesize[2] = input_data.raw()->linesize[2];
            log_debug("Sending... " << output_data);
            try_to(sendOutputData(output_data));
        }
    }

} // namespace fpp
