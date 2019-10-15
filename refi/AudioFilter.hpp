#pragma once
#include "base/Filter.hpp"

namespace fpp {

    class AudioFilter : public Filter {

    public:

        AudioFilter(StreamPair io_streams, std::string filters_descr);

    private:

        Code processInputData(Frame& input_data);

    };

} // namespace fpp
