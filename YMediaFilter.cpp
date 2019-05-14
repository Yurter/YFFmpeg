#include "YMediaFilter.h"
#include <iostream>

YMediaFilter::YMediaFilter(std::string filter_description) :
    _filter_description(filter_description)
{
    //
}

YMediaFilter::~YMediaFilter()
{
    //
}

bool YMediaFilter::init(YMediaSource *source, YMediaDecoder *decoder)
{
    std::cout << "[YMediaFilter] Initializing" << std::endl;



    AVFormatContext *fmt_ctx = source->mediaFormatContext();
    AVCodecContext *dec_ctx = decoder->videoCodecContext();




    char args[512]; //TODO
    int ret = 0;
    const AVFilter *buffersrc  = avfilter_get_by_name("buffer");
    const AVFilter *buffersink = avfilter_get_by_name("buffersink");
    AVFilterInOut *outputs = avfilter_inout_alloc();
    AVFilterInOut *inputs  = avfilter_inout_alloc();
    AVRational time_base = fmt_ctx->streams[AVMEDIA_TYPE_VIDEO]->time_base;
    enum AVPixelFormat pix_fmts[] = { AV_PIX_FMT_GRAY8, AV_PIX_FMT_NONE };

    _filter_graph = avfilter_graph_alloc();
    if (!outputs || !inputs || !_filter_graph) {
        ret = AVERROR(ENOMEM);
        return false;
    }

    /* buffer video source: the decoded frames from the decoder will be inserted here. */
    snprintf(args, sizeof(args),
            "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
            dec_ctx->width, dec_ctx->height, dec_ctx->pix_fmt,
            time_base.num, time_base.den,
            dec_ctx->sample_aspect_ratio.num, dec_ctx->sample_aspect_ratio.den);

    ret = avfilter_graph_create_filter(&_buffersrc_ctx, buffersrc, "in",
                                       args, nullptr, _filter_graph);
    if (ret < 0) {
        av_log(nullptr, AV_LOG_ERROR, "Cannot create buffer source\n");
        return false;
    }

    /* buffer video sink: to terminate the filter chain. */
    ret = avfilter_graph_create_filter(&_buffersink_ctx, buffersink, "out",
                                       nullptr, nullptr, _filter_graph);
    if (ret < 0) {
        av_log(nullptr, AV_LOG_ERROR, "Cannot create buffer sink\n");
        return false;
    }

    ret = av_opt_set_int_list(_buffersink_ctx, "pix_fmts", pix_fmts,
                              AV_PIX_FMT_NONE, AV_OPT_SEARCH_CHILDREN);
    if (ret < 0) {
        av_log(nullptr, AV_LOG_ERROR, "Cannot set output pixel format\n");
        return false;
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

    if ((ret = avfilter_graph_parse_ptr(_filter_graph, _filter_description.c_str(),
                                        &inputs, &outputs, nullptr)) < 0) {
        return false;
    }

    if ((ret = avfilter_graph_config(_filter_graph, nullptr)) < 0) {
        return false;
    }


    avfilter_inout_free(&inputs); //TODO on return false;
    avfilter_inout_free(&outputs);



    return true;
}

bool YMediaFilter::filterFrames(std::list<AVFrame*> &frames)
{

    std::list<AVFrame*> filtered_frames;

    for (auto& frame : frames) {

        int ret;

        /* push the decoded frame into the filtergraph */
        if (av_buffersrc_add_frame_flags(_buffersrc_ctx, frame, AV_BUFFERSRC_FLAG_KEEP_REF) < 0) {
            av_log(nullptr, AV_LOG_ERROR, "Error while feeding the filtergraph\n");
            return false;
        }


        /* pull filtered frames from the filtergraph */
        while (1) {
            AVFrame *filt_frame = av_frame_alloc();
            ret = av_buffersink_get_frame(_buffersink_ctx, filt_frame);
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                break;
            if (ret < 0)
                break;

            filtered_frames.push_back(filt_frame);


        }

    }


    frames = std::move(filtered_frames);

    return true;
}
