#include "AudioStream.hpp"
#include "../base/utils.hpp"

namespace fpp {

    AudioStream::AudioStream(AudioParameters* param) :
        AudioStream(nullptr, param)
    {
        EMPTY_CONSTRUCTOR
    }

    AudioStream::AudioStream(AVStream* stream, AudioParameters* param) :
        Stream(stream, param)
    {
        setName("AudioStream");
    }

    Code AudioStream::init() //TODO
    {
        try_to(Stream::init());
    //    int64_t duration = av_rescale_q(int64_t(1000 / parameters.frameRate())
    //                        , {1, 1000}, timeBase());
        int64_t duration = 23; //TODO
        _packet_dts_delta = duration;
        _packet_pts_delta = duration;
        _packet_duration = duration;
        setInited(true);
        return Code::OK;
    }

    bool AudioStream::operator>(const Stream& other) const {
        auto this_params = static_cast<AudioParameters*>(parameters);
        auto other_params = static_cast<AudioParameters*>(other.parameters);
        return this_params->bitrate() > other_params->bitrate();
    }

} // namespace fpp
