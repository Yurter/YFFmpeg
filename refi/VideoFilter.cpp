#include "VideoFilter.hpp"

namespace fpp {

    VideoFilter::VideoFilter(IOParams params, std::string filters_descr) :
        Filter(params, filters_descr)
    {
        setName("VideoFilter");
    }

} // namespace fpp
