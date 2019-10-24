#include "AudioFilter.hpp"

namespace fpp {

    AudioFilter::AudioFilter(const Parameters *input_data_params, std::string filters_descr) :
        Filter(input_data_params, filters_descr)
    {
        setName("AudioFilter");
        setSkipType(MediaType::MEDIA_TYPE_VIDEO);
    }

    Code AudioFilter::processInputData(Frame input_data) {
        UNUSED(input_data);
        return Code::NOT_IMPLEMENTED;
    }

} // namespace fpp
