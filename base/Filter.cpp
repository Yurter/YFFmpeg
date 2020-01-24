#include "Filter.hpp"

namespace ffmpeg { extern "C" {
    #include <libavfilter/avfilter.h>
    #include <libavfilter/buffersink.h>
    #include <libavfilter/buffersrc.h>
    #include <libavutil/opt.h>
} } // namespace ffmpeg


namespace fpp {

    Filter::Filter(IOParams params, const std::string& filters_descr)
        : params(params)
        , _filters_descr(filters_descr) {
        setName("Filter");
    }

    Filter::~Filter() {
        try_throw(stop());
    }

    void Filter::init() {
        using namespace ffmpeg;
//        return_if(inited(), Code::OK);
        auto out_params = dynamic_cast<const VideoParameters*>(params.out.get()); //TODO решить использовать входные или выходные параметры

        char args[512];
        int ret = 0;
        const AVFilter* buffersrc  = avfilter_get_by_name("buffer");
        const AVFilter* buffersink = avfilter_get_by_name("buffersink");
        AVFilterInOut* outputs = avfilter_inout_alloc(); //TODO убрать голый указатель 20.01
        AVFilterInOut* inputs  = avfilter_inout_alloc(); //TODO убрать голый указатель 20.01
        AVRational time_base = out_params->timeBase();
//        enum AVPixelFormat pix_fmts[] = { AV_PIX_FMT_YUV420P, AV_PIX_FMT_NONE };
        enum AVPixelFormat pix_fmts[] = { out_params->pixelFormat(), AV_PIX_FMT_NONE };

        _filter_graph = avfilter_graph_alloc();
        if (!outputs || !inputs || !_filter_graph) {
            ret = AVERROR(ENOMEM);
//            return Code::FFMPEG_ERROR;
        }

        /* buffer video source: the decoded frames from the decoder will be inserted here. */
        snprintf(args, sizeof(args),
                "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
                int(out_params->width()), int(out_params->height()), out_params->pixelFormat(),
                time_base.num, time_base.den,
                out_params->aspectRatio().num, out_params->aspectRatio().den);

        ret = avfilter_graph_create_filter(&_buffersrc_ctx, buffersrc, "in",
                                           args, nullptr, _filter_graph);
        if (ret < 0) {
            av_log(nullptr, AV_LOG_ERROR, "Cannot create buffer source\n");
//            return Code::FFMPEG_ERROR;
        }

        log_info("Filter in inited with args: " << args);

        /* buffer video sink: to terminate the filter chain. */
        ret = avfilter_graph_create_filter(&_buffersink_ctx, buffersink, "out",
                                           nullptr, nullptr, _filter_graph);
        if (ret < 0) {
            av_log(nullptr, AV_LOG_ERROR, "Cannot create buffer sink\n");
//            return Code::FFMPEG_ERROR;
        }

        snprintf(args, sizeof(args), "NULL");
        log_info("Filter out inited with args: " << args);

        ret = av_opt_set_int_list(_buffersink_ctx, "pix_fmts", pix_fmts,
                                  AV_PIX_FMT_NONE, AV_OPT_SEARCH_CHILDREN);
        if (ret < 0) {
            av_log(nullptr, AV_LOG_ERROR, "Cannot set output pixel format\n");
//            return Code::FFMPEG_ERROR;
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
                                            &inputs, &outputs, nullptr)) < 0) {
//            return Code::FFMPEG_ERROR;
        }

        log_info("Filter description: " << _filters_descr);

        if ((ret = avfilter_graph_config(_filter_graph, nullptr)) < 0) {
//            return Code::FFMPEG_ERROR;
        }

        avfilter_inout_free(&inputs);
        avfilter_inout_free(&outputs);

        setInited(true);
//        return Code::OK;
    }

    Code Filter::open() {
        setOpened(true);
        return Code::OK;
    }

    Code Filter::close() {
        setOpened(false);
        return Code::OK;
    }

    bool Filter::equalTo(const SharedProcessor other) const {
        auto other_filter = dynamic_cast<const Filter * const>(other.get());
        return_if(not_inited_ptr(other_filter), false);

        return_if(this->description()
                  != other_filter->description(), false);
        return_if(this->params.out->streamUid()
                  != other_filter->params.out->streamUid(), false);

        return true;
    }

    std::string Filter::description() const {
        return _filters_descr;
    }

    Code Filter::processInputData(Frame input_data) { //TODO много параметров захардкожено 14.01
        if (av_buffersrc_add_frame_flags(_buffersrc_ctx, &input_data.raw(), ffmpeg::AV_BUFFERSRC_FLAG_KEEP_REF) < 0) {
            log_error("Error while feeding the filtergraph: " << input_data);
            return Code::FFMPEG_ERROR;
        }
        /* pull filtered frames from the filtergraph */
        while (1) {
            Frame output_data;
            int ret = ffmpeg::av_buffersink_get_frame(_buffersink_ctx, &output_data.raw());
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                return Code::AGAIN;
            if (ret < 0)
                return Code::FFMPEG_ERROR;
            output_data.setType(MediaType::Video);
            output_data.raw().linesize[0] = input_data.raw().linesize[0]; //TODO убрать?
            output_data.raw().linesize[1] = input_data.raw().linesize[1];
            output_data.raw().linesize[2] = input_data.raw().linesize[2];

            try_to(sendOutputData(output_data));
        }
    }

} // namespace fpp
