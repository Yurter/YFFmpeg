#include "YMediaFilter.h"
#include <iostream>

YMediaFilter::YMediaFilter()
{
    //
}

bool YMediaFilter::init()
{
    std::cout << "[YMediaFilter] Initializing" << std::endl;
    return true;
}

std::list<AVFrame> YMediaFilter::filterFrames(std::list<AVFrame> &&frames)
{
    std::list<AVFrame> frame_list;
    return frame_list;
}

bool YMediaFilter::apply(AVPacket packet)
{
	//int ret;
	//this;
	//AVFrame in_frame;
	//int got = 0;

	//AVPacket test_packet;

	//if (av_read_frame(_format_context, &test_packet) < 0) {
	//	return false;
	//}	

	//int idx = test_packet.stream_index;
	//if (idx != 0) {
	//	return false;
	//}

	//if ((ret = avcodec_send_packet(pCodecCtx, &test_packet)) < 0) {
	//	std::cerr << AVERROR(ret) << " Error1" << std::endl;
	//}

	//if (avcodec_receive_frame(pCodecCtx, &in_frame) != 0) {
	//	std::cerr << "Error2" << std::endl;
	//}

	////ret = avcodec_decode_video2(pCodecCtx, &in_frame, &got, packet);
	//if (ret < 0) {
	//	av_log(NULL, AV_LOG_ERROR, "Error decoding video\n");
	//	return false;
	//}
	//if (av_buffersrc_add_frame(buffersrc_ctx, &in_frame) < 0) {
	//	av_log(NULL, AV_LOG_ERROR, "Error while feeding the filtergraph\n");
	//	return false;
	//}
	//AVFrame out_frame;
	//if (av_buffersink_get_frame(buffersink_ctx, &out_frame) < 0) {
	//	return false;
	//}
	//if (avcodec_encode_video2(pCodecCtx, packet, &out_frame, nullptr)) {
	//	return false;
	//}
	//return true;
	///////////////////////////////////////////////////////////////////////////



	int ret = 0;
	AVFrame *frame;
	AVFrame *filt_frame;
	frame = av_frame_alloc();
	filt_frame = av_frame_alloc();

	ret = avcodec_send_packet(pCodecCtx, &packet);
	if (ret < 0) {
		av_log(NULL, AV_LOG_ERROR, "[!!!] Error while sending a packet to the decoder\n");
		return false;
	}

	while (ret >= 0) {
		//                cout << "====================== " << dec_ctx << endl;
		ret = avcodec_receive_frame(pCodecCtx, frame);
		if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
			break;
		}
		else if (ret < 0) {
			av_log(NULL, AV_LOG_ERROR, "Error while receiving a frame from the decoder\n");
			return false;
		}

		frame->pts = frame->best_effort_timestamp;

		/* push the decoded frame into the filtergraph */
		if (av_buffersrc_add_frame_flags(buffersrc_ctx, frame, AV_BUFFERSRC_FLAG_KEEP_REF) < 0) {
			av_log(NULL, AV_LOG_ERROR, "Error while feeding the filtergraph\n");
			break;
		}

		/* pull filtered frames from the filtergraph */
		while (1) {
			ret = av_buffersink_get_frame(buffersink_ctx, filt_frame);
			if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
				break;
			if (ret < 0)
				return false;
		}
	}
	return true;
}

YMediaFilter::~YMediaFilter()
{
	//
}

int YMediaFilter::init(AVCodecContext *codec_context, AVFormatContext *format_context)
{
	//_format_context = format_context;
	//pCodecCtx = codec_context;
	//const char *filter_descr = "scale=120:120"; //"movie=my_logo.png[wm];[in][wm]overlay=5:5[out]";
	//char args[512];
	//int ret;
	//const AVFilter *buffersrc = avfilter_get_by_name("buffer");
	//const AVFilter *buffersink = avfilter_get_by_name("buffersink");
	//AVFilterInOut *outputs = avfilter_inout_alloc();
	//AVFilterInOut *inputs = avfilter_inout_alloc();
	//enum AVPixelFormat pix_fmts[] = { AV_PIX_FMT_GRAY8, AV_PIX_FMT_NONE };
	//AVBufferSinkParams *buffersink_params;

	//filter_graph = avfilter_graph_alloc();

	///* buffer video source: the decoded frames from the decoder will be inserted here. */
	////_snprintf(args, sizeof(args),
	////	"video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
	////	pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
	////	pCodecCtx->time_base.num, pCodecCtx->time_base.den,
	////	pCodecCtx->sample_aspect_ratio.num, pCodecCtx->sample_aspect_ratio.den);

	//_snprintf(args, sizeof(args),
	//	"video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
	//	1920, 1080, AV_PIX_FMT_YUV420P,
	//	29, 1,
	//	1, 1);

	//ret = avfilter_graph_create_filter(&buffersrc_ctx, buffersrc, "in", args, NULL, filter_graph);
	//if (ret < 0) {
	//	av_log(NULL, AV_LOG_ERROR, "Cannot create buffer source\n");
	//	return ret;
	//}

	///* buffer video sink: to terminate the filter chain. */
	//buffersink_params = av_buffersink_params_alloc();
	//buffersink_params->pixel_fmts = pix_fmts;
	//ret = avfilter_graph_create_filter(&buffersink_ctx, buffersink, "out", NULL, buffersink_params, filter_graph);
	//av_free(buffersink_params);
	//if (ret < 0) {
	//	av_log(NULL, AV_LOG_ERROR, "Cannot create buffer sink\n");
	//	return ret;
	//}

	///* Endpoints for the filter graph. */
	//outputs->name = av_strdup("in");
	//outputs->filter_ctx = buffersrc_ctx;
	//outputs->pad_idx = 0;
	//outputs->next = NULL;

	//inputs->name = av_strdup("out");
	//inputs->filter_ctx = buffersink_ctx;
	//inputs->pad_idx = 0;
	//inputs->next = NULL;

	//if ((ret = avfilter_graph_parse(filter_graph, filter_descr, inputs, outputs, NULL)) < 0)
	//	return ret;

	//if ((ret = avfilter_graph_config(filter_graph, NULL)) < 0)
	//	return ret;
	//return 0;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////

	pCodecCtx = codec_context;
	_format_context = format_context;
	char args[512];
	int ret = 0;
	const AVFilter *buffersrc = avfilter_get_by_name("buffer");
	const AVFilter *buffersink = avfilter_get_by_name("buffersink");
	AVFilterInOut *outputs = avfilter_inout_alloc();
	AVFilterInOut *inputs = avfilter_inout_alloc();
	AVRational time_base = {1000,1};// format_context->streams[AVMEDIA_TYPE_VIDEO]->time_base;
	enum AVPixelFormat pix_fmts[] = { AV_PIX_FMT_GRAY8, AV_PIX_FMT_NONE };

	filter_graph = avfilter_graph_alloc();
	if (!outputs || !inputs || !filter_graph) {
		ret = AVERROR(ENOMEM);
		goto end;
	}

	/* buffer video source: the decoded frames from the decoder will be inserted here. */
	snprintf(args, sizeof(args),
		"video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
		codec_context->width, codec_context->height, codec_context->pix_fmt,
		time_base.num, time_base.den,
		codec_context->sample_aspect_ratio.num, codec_context->sample_aspect_ratio.den);

	ret = avfilter_graph_create_filter(&buffersrc_ctx, buffersrc, "in",
		args, NULL, filter_graph);
	if (ret < 0) {
		av_log(NULL, AV_LOG_ERROR, "Cannot create buffer source\n");
		goto end;
	}

	/* buffer video sink: to terminate the filter chain. */
	ret = avfilter_graph_create_filter(&buffersink_ctx, buffersink, "out",
		NULL, NULL, filter_graph);
	if (ret < 0) {
		av_log(NULL, AV_LOG_ERROR, "Cannot create buffer sink\n");
		goto end;
	}

	ret = av_opt_set_int_list(buffersink_ctx, "pix_fmts", pix_fmts,
		AV_PIX_FMT_NONE, AV_OPT_SEARCH_CHILDREN);
	if (ret < 0) {
		av_log(NULL, AV_LOG_ERROR, "Cannot set output pixel format\n");
		goto end;
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
	outputs->name = av_strdup("in");
	outputs->filter_ctx = buffersrc_ctx;
	outputs->pad_idx = 0;
	outputs->next = NULL;

	/*
	 * The buffer sink input must be connected to the output pad of
	 * the last filter described by filters_descr; since the last
	 * filter output label is not specified, it is set to "out" by
	 * default.
	 */
	inputs->name = av_strdup("out");
	inputs->filter_ctx = buffersink_ctx;
	inputs->pad_idx = 0;
	inputs->next = NULL;

	const char *filter_descr = "scale=140:-1";

	if ((ret = avfilter_graph_parse_ptr(filter_graph, filter_descr,
		&inputs, &outputs, NULL)) < 0)
		goto end;

	if ((ret = avfilter_graph_config(filter_graph, NULL)) < 0)
		goto end;

end:
	avfilter_inout_free(&inputs);
	avfilter_inout_free(&outputs);

	return ret;
}
