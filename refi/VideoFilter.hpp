#pragma once
#include <base/Filter.hpp>

namespace fpp {

    class VideoFilter : public Filter {

    public:

        VideoFilter(SharedParameters parameters, std::string filters_descr);

    };

} // namespace fpp
