#include "AudioFilter.hpp"

namespace fpp {

    AudioFilter::AudioFilter(StreamPair io_streams, std::string filters_descr) :
        Filter(io_streams, filters_descr)
    {
        setName("AudioFilter");
        setSkipType(MediaType::MEDIA_TYPE_VIDEO);
    }

    Code AudioFilter::processInputData(Frame& input_data) {
        return Code::NOT_INITED;
    }

} // namespace fpp
