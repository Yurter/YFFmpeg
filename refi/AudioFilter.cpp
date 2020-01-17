#include "AudioFilter.hpp"

namespace fpp {

    AudioFilter::AudioFilter(IOParams params, std::string filters_descr) :
        Filter(params, filters_descr)
    {
        setName("AudioFilter");
    }

    Code AudioFilter::processInputData(Frame input_data) {
        return Code::NOT_IMPLEMENTED;
    }

} // namespace fpp
