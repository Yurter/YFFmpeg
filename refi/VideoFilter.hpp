#pragma once
#include "base/Filter.hpp"

namespace fpp {

    class VideoFilter : public Filter {

    public:

        VideoFilter(IOParams params, std::string filters_descr);

    };

} // namespace fpp
