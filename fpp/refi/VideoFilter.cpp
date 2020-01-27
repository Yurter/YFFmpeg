#include "VideoFilter.hpp"

namespace fpp {

    VideoFilter::VideoFilter(SharedParameters parameters, std::string filters_descr) :
        Filter(parameters, filters_descr)
    {
        setName("VideoFilter");
    }

} // namespace fpp
