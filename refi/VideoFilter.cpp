#include "VideoFilter.hpp"

namespace fpp {

    VideoFilter::VideoFilter(StreamPair io_streams, std::string filters_descr) :
        Filter(io_streams, filters_descr)
    {
        setName("VideoFilter");
        setSkipType(MediaType::MEDIA_TYPE_AUDIO);
    }

} // namespace fpp
