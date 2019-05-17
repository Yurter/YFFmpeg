#include "YMediaFilter.h"
#include <iostream>

YMediaFilter::YMediaFilter(AVMediaType filter_type, std::string filter_description) :
    _inited(false),
    _filter_type(filter_type),
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
    switch (_filter_type) {
    case AVMEDIA_TYPE_VIDEO:
        return initVideoFilter(source, decoder);
    case AVMEDIA_TYPE_AUDIO:
        return initAudioFilter(source, decoder);
    default:
        return false;
    }
}

bool YMediaFilter::filterFrames(std::list<AVFrame*> &frames)
{
    if (!_inited) { return false; }




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
            filt_frame = av_frame_alloc();

        }

    }

    frames = std::move(filtered_frames);

    return true;
}

bool YMediaFilter::initVideoFilter(YMediaSource *source, YMediaDecoder *decoder)
{
    AVFormatContext *fmt_ctx = source->mediaFormatContext();
    AVCodecContext *dec_ctx = decoder->videoCodecContext();




    char args[512]; //TODO
    int ret = 0;
    const AVFilter *buffersrc  = avfilter_get_by_name("buffer");
    const AVFilter *buffersink = avfilter_get_by_name("buffersink");
    AVFilterInOut *outputs = avfilter_inout_alloc();
    AVFilterInOut *inputs  = avfilter_inout_alloc();
    AVRational time_base = fmt_ctx->streams[AVMEDIA_TYPE_VIDEO]->time_base;
    enum AVPixelFormat pix_fmts[] = { AV_PIX_FMT_YUV420P }; //{ AV_PIX_FMT_GRAY8, AV_PIX_FMT_NONE };

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


    _inited = true;
    return true;
}

bool YMediaFilter::initAudioFilter(YMediaSource *source, YMediaDecoder *decoder)
{

    AVFormatContext *fmt_ctx = source->mediaFormatContext();
    AVCodecContext *dec_ctx = decoder->audioCodecContext();


    char args[512];
    int ret;
    const AVFilter *abuffersrc  = avfilter_get_by_name("abuffer");
    const AVFilter *abuffersink = avfilter_get_by_name("abuffersink");
    AVFilterInOut *outputs = avfilter_inout_alloc();
    AVFilterInOut *inputs  = avfilter_inout_alloc();
    static const enum AVSampleFormat out_sample_fmts[] = { AV_SAMPLE_FMT_S16 }; //{ AV_SAMPLE_FMT_S16, AV_SAMPLE_FMT_NONE };
    static const int64_t out_channel_layouts[] = { AV_CH_LAYOUT_MONO, -1 };
    static const int out_sample_rates[] = { 8000, -1 };
    const AVFilterLink *outlink;
    AVRational time_base = fmt_ctx->streams[AVMEDIA_TYPE_AUDIO]->time_base;
    _filter_graph = avfilter_graph_alloc();
    /* buffer audio source: the decoded frames from the decoder will be inserted here. */
    if (!dec_ctx->channel_layout)
        dec_ctx->channel_layout = av_get_default_channel_layout(dec_ctx->channels);
    const char* debug_value = av_get_sample_fmt_name(dec_ctx->sample_fmt);
    snprintf(args, sizeof(args),
            "time_base=%d/%d:sample_rate=%d:sample_fmt=%s:channel_layout=0x%d",
             time_base.num, time_base.den, dec_ctx->sample_rate,
             av_get_sample_fmt_name(dec_ctx->sample_fmt), dec_ctx->channel_layout);
    ret = avfilter_graph_create_filter(&_buffersrc_ctx, abuffersrc, "in",
                                       args, NULL, _filter_graph);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot create audio buffer source\n");
        return ret;
    }
    /* buffer audio sink: to terminate the filter chain. */
    ret = avfilter_graph_create_filter(&_buffersink_ctx, abuffersink, "out",
                                       NULL, NULL, _filter_graph);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot create audio buffer sink\n");
        return ret;
    }
    ret = av_opt_set_int_list(_buffersink_ctx, "sample_fmts", out_sample_fmts, -1,
                              AV_OPT_SEARCH_CHILDREN);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot set output sample format\n");
        return ret;
    }
    ret = av_opt_set_int_list(_buffersink_ctx, "channel_layouts", out_channel_layouts, -1,
                              AV_OPT_SEARCH_CHILDREN);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot set output channel layout\n");
        return ret;
    }
    ret = av_opt_set_int_list(_buffersink_ctx, "sample_rates", out_sample_rates, -1,
                              AV_OPT_SEARCH_CHILDREN);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot set output sample rate\n");
        return ret;
    }
    /* Endpoints for the filter graph. */
    outputs->name       = av_strdup("in");
    outputs->filter_ctx = _buffersrc_ctx;
    outputs->pad_idx    = 0;
    outputs->next       = NULL;
    inputs->name       = av_strdup("out");
    inputs->filter_ctx = _buffersink_ctx;
    inputs->pad_idx    = 0;
    inputs->next       = NULL;
    if ((ret = avfilter_graph_parse_ptr(_filter_graph, _filter_description.c_str(),
                                    &inputs, &outputs, NULL)) < 0)
        return ret;
    if ((ret = avfilter_graph_config(_filter_graph, NULL)) < 0)
        return ret;
    /* Print summary of the sink buffer
     * Note: args buffer is reused to store channel layout string */
    outlink = _buffersink_ctx->inputs[0];
    av_get_channel_layout_string(args, sizeof(args), -1, outlink->channel_layout);
//    av_log(NULL, AV_LOG_INFO, "Output: srate:%dHz fmt:%s chlayout:%s\n",
//           (int)outlink->sample_rate,
//           (char *)av_x_if_null(av_get_sample_fmt_name(outlink->format), "?"),
//           args);

    _inited = true;
    return true;
}
