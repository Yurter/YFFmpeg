#include "VideoFilter.hpp"

namespace fpp {

    VideoFilter::VideoFilter(const Parameters* input_data_params, std::string filters_descr) :
        Filter(input_data_params, filters_descr)
    {
        setName("VideoFilter");
    }

} // namespace fpp
