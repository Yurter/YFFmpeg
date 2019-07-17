//#pragma once

//#include "YVideoDecoder.hpp"

//class YMediaFilter
//{

//public:

//    YMediaFilter(AVMediaType filter_type, std::string filter_description);
//    ~YMediaFilter();

//    bool init(YSource *source, YDecoder *decoder);

//    bool filterFrames(std::list<AVFrame*> &frames);

//private:

//    bool initVideoFilter(YSource *source, YDecoder *decoder);
//    bool initAudioFilter(YSource *source, YDecoder *decoder);

//private:

//    // General parameters
//    bool                _inited;
//    AVMediaType         _filter_type;
//    std::string         _filter_description;

//    // FFmpeg
//    AVFilterContext     *_buffersrc_ctx;
//    AVFilterContext     *_buffersink_ctx;
//    AVFilterGraph       *_filter_graph;

//};

