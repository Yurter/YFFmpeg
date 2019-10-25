#pragma once
#include "base/Filter.hpp"

namespace fpp {

    class VideoFilter : public Filter {

    public:

        VideoFilter(const Parameters* input_data_params, std::string filters_descr);

    };

} // namespace fpp
