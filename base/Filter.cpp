#include "Filter.hpp"

namespace fpp {

    Filter::Filter(IOParams params, std::string filters_descr) :
        params(params)
      , _filters_descr(filters_descr)
    {
        setName("Filter");
    }

    Filter::~Filter() {
        join(); //TODO вынести в наследники, если они остануться
    }

    Code Filter::init() {
        return_if(inited(), Code::INVALID_CALL_ORDER);
        auto out_params = dynamic_cast<const VideoParameters*>(params.out); //TODO решить использовать входные или выходные параметры

        char args[512];
        int ret = 0;
        const AVFilter *buffersrc  = avfilter_get_by_name("buffer");
        const AVFilter *buffersink = avfilter_get_by_name("buffersink");
        AVFilterInOut *outputs = avfilter_inout_alloc();
        AVFilterInOut *inputs  = avfilter_inout_alloc();
        AVRational time_base = out_params->timeBase();
//        enum AVPixelFormat pix_fmts[] = { AV_PIX_FMT_YUV420P, AV_PIX_FMT_NONE };
        enum AVPixelFormat pix_fmts[] = { out_params->pixelFormat(), AV_PIX_FMT_NONE };

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

        ret = avfilter_graph_create_filter(&_buffersrc_ctx, buffersrc, "in",
                                           args, nullptr, _filter_graph);
        if (ret < 0) {
            av_log(nullptr, AV_LOG_ERROR, "Cannot create buffer source\n");
            return Code::FFMPEG_ERROR;
        }

        log_info("Filter in inited with args: " << args);

        /* buffer video sink: to terminate the filter chain. */
        ret = avfilter_graph_create_filter(&_buffersink_ctx, buffersink, "out",
                                           nullptr, nullptr, _filter_graph);
        if (ret < 0) {
            av_log(nullptr, AV_LOG_ERROR, "Cannot create buffer sink\n");
            return Code::FFMPEG_ERROR;
        }

        snprintf(args, sizeof(args), "NULL");
        log_info("Filter out inited with args: " << args);

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
                                            &inputs, &outputs, nullptr)) < 0) {
            return Code::FFMPEG_ERROR;
        }

        log_info("Filter description: " << _filters_descr);

        if ((ret = avfilter_graph_config(_filter_graph, nullptr)) < 0)
            return Code::FFMPEG_ERROR;

        avfilter_inout_free(&inputs);
        avfilter_inout_free(&outputs);

        //return ret;
        setInited(true);
        return Code::OK;
    }

    Code Filter::open() {
        setOpened(true);
        return Code::OK;
    }

    Code Filter::close() {
        setOpened(false);
        return Code::OK;
    }

    bool Filter::equalTo(const ProcessorPointer other) const {
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

    Code Filter::processInputData(Frame input_data) {
        if (av_buffersrc_add_frame_flags(_buffersrc_ctx, &input_data.raw(), AV_BUFFERSRC_FLAG_KEEP_REF) < 0) {
            log_error("Error while feeding the filtergraph: " << input_data);
            return Code::FFMPEG_ERROR;
        }
        /* pull filtered frames from the filtergraph */
        while (1) {
//            AVFrame* filt_frame = av_frame_alloc();
            Frame output_data;
            int ret = av_buffersink_get_frame(_buffersink_ctx, &output_data.raw());
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                return Code::AGAIN;
            if (ret < 0)
                return Code::FFMPEG_ERROR;
//            Frame output_data(filt_frame);
            output_data.setType(MEDIA_TYPE_VIDEO);
            output_data.raw().linesize[0] = input_data.raw().linesize[0];
            output_data.raw().linesize[1] = input_data.raw().linesize[1];
            output_data.raw().linesize[2] = input_data.raw().linesize[2];

//            log_error("in_px_fmt " << input_data.raw().format);
//            log_error("out_px_fmt " << output_data.raw().format);

//            output_data.raw().linesize[0] = output_data.raw().width;
//            output_data.raw().linesize[1] = output_data.raw().width;
//            output_data.raw().linesize[2] = output_data.raw().height;

//            std::begin(input_data.raw().linesize)
//            for (int i = 0; i < AV_NUM_DATA_POINTERS; ++i) {
////                log_info("output_data.raw().linesize[i] = " << output_data.raw().linesize[i]);
////                log_info("input_data.raw().linesize[i] = " << input_data.raw().linesize[i]);
//                output_data.raw().linesize[i] = input_data.raw().linesize[i];
//            }
//            av_frame_copy_props(&output_data.raw(), &input_data.raw());
//            utils::SaveAvFrame(&output_data.raw());
//            exit(0);
//            input_data.free();
//            log_error("Sending... " << output_data);

            //  D E B U G - падения при отправке отфильтрованного фрейма

            output_data.setPts(output_data.pts() / 60); //TODO CRITICAL фильтр на пропуск кадров не меняет птс фреймов, поэтому на выходе птсы 0го, 60го, 120 и тд, а не 0, 1, 2..
//            log_debug(output_data);
            try_to(sendOutputData(output_data));
//            try_to(sendOutputData(input_data));
        }
    }

} // namespace fpp
