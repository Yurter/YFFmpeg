#pragma once

#include "YDecoder.h"

class YMediaFilter
{

public:

    YMediaFilter(AVMediaType filter_type, std::string filter_description);
    ~YMediaFilter();

    bool init(YMediaSource *source, YMediaDecoder *decoder);

    bool filterFrames(std::list<AVFrame*> &frames);

private:

    bool initVideoFilter(YMediaSource *source, YMediaDecoder *decoder);
    bool initAudioFilter(YMediaSource *source, YMediaDecoder *decoder);

private:

    // General parameters
    bool                _inited;
    AVMediaType         _filter_type;
    std::string         _filter_description;

    // FFmpeg
    AVFilterContext     *_buffersrc_ctx;
    AVFilterContext     *_buffersink_ctx;
    AVFilterGraph       *_filter_graph;

};

