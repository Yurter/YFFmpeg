#include "VideoStream.hpp"
#include "../base/utils.hpp"

namespace fpp {

    VideoStream::VideoStream(VideoParameters* param) :
        VideoStream(nullptr, param)
    {
        EMPTY_CONSTRUCTOR
    }

    VideoStream::VideoStream(AVStream* stream, VideoParameters* param) :
        Stream(stream, param)
    {
        setName("VideoStream");
    }

    Code VideoStream::init() {
        try_to(Stream::init());
        auto video_parameers = dynamic_cast<VideoParameters*>(parameters);
        return_if(invalid_rational(parameters->timeBase()), Code::INVALID_INPUT);
    //    log_debug("video_parameers->frameRate() = " << video_parameers->frameRate());
    //    log_debug("timeBase() = " << utils::rational_to_string(timeBase()));
        int64_t packet_duration = av_rescale_q(int64_t(1000 / video_parameers->frameRate())
                                    , {1, 1000}, parameters->timeBase());
        _packet_dts_delta = packet_duration;
        _packet_pts_delta = packet_duration;
        _packet_duration = packet_duration;
        setInited(true);
        return Code::OK;
    }

    bool VideoStream::operator>(const Stream& other) const {
        auto this_params = static_cast<VideoParameters*>(parameters);
        auto other_params = static_cast<VideoParameters*>(other.parameters);
        auto this_pixels = this_params->width() * this_params->height();
        auto other_pixels = other_params->width() * other_params->height();
        return this_pixels > other_pixels;
    }

} // namespace fpp
