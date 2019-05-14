#pragma once

#include "ffmpeg.h"
#include "YMediaDecoder.h"

class YMediaFilter
{

public:

    YMediaFilter(std::string filter_description);
    ~YMediaFilter();

    bool init(YMediaSource *source, YMediaDecoder *decoder);

    bool filterFrames(std::list<AVFrame*> &frames);
	bool apply(AVPacket packet);


//private:

	//
	int init(AVCodecContext *codec_context, AVFormatContext *format_context);

private:

    // General parameters
    std::string         _filter_description;

    // FFmpeg
    AVFilterContext     *_buffersrc_ctx;
    AVFilterContext     *_buffersink_ctx;
    AVFilterGraph       *_filter_graph;

};

