#include "YVideoStream.hpp"
#include "../base/utils.hpp"

YVideoStream::YVideoStream(YVideoParameters* param) :
    YVideoStream(nullptr, param)
{
    EMPTY_CONSTRUCTOR
}

YVideoStream::YVideoStream(AVStream* stream, YVideoParameters* param) :
    YStream(stream, param)
{
    setName("YVideoStream");
}

YCode YVideoStream::init()
{
    try_to(YStream::init());
    auto video_parameers = dynamic_cast<YVideoParameters*>(parameters);
    return_if(invalid_rational(parameters->timeBase()), YCode::INVALID_INPUT);
//    log_debug("video_parameers->frameRate() = " << video_parameers->frameRate());
//    log_debug("timeBase() = " << utils::rational_to_string(timeBase()));
    int64_t packet_duration = av_rescale_q(int64_t(1000 / video_parameers->frameRate())
                                , {1, 1000}, parameters->timeBase());
    _packet_dts_delta = packet_duration;
    _packet_pts_delta = packet_duration;
    _packet_duration = packet_duration;
    setInited(true);
    return YCode::OK;
}
