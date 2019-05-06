#pragma once

#include "YMediaDecoder.h"

extern "C" {
#include "libavutil/avutil.h"
#include "libavutil/pixfmt.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavfilter/buffersink.h"
#include "libavfilter/buffersrc.h"
#include "libavfilter/avfilter.h"
#include "libswscale/swscale.h"
#include <libavutil/opt.h>
};


class YMediaFilter
{
public:

	YMediaFilter();

    bool init();

    std::list<AVFrame> filterFrames(std::list<AVFrame> &&frames);
	bool apply(AVPacket packet);

	~YMediaFilter();

//private:

	//
	int init(AVCodecContext *codec_context, AVFormatContext *format_context);

private:

	//

	AVFormatContext *pFormatCtx;
	AVCodecContext *pCodecCtx;
	AVFilterContext *buffersink_ctx;
	AVFilterContext *buffersrc_ctx;
	AVFilterGraph *filter_graph;
	static int video_stream_index;
	static int64_t last_pts;

	AVFormatContext *_format_context;

};

