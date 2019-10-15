#pragma once
#include "base/Filter.hpp"

namespace fpp {

    class VideoFilter : public Filter {

    public:

        VideoFilter(StreamPair io_streams, std::string filters_descr);

    };

} // namespace fpp
