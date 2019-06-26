#include "YVideoStream.h"

YVideoStream::YVideoStream(YVideoParameters param) :
    YVideoStream(nullptr, param)
{
    //
}

YVideoStream::YVideoStream(AVStream* stream, YVideoParameters param) :
    YStream(stream, YMediaType::MEDIA_TYPE_VIDEO, param)
{
    setName("YVideoStream");
}

YCode YVideoStream::init()
{
    auto video_parameers = dynamic_cast<YVideoParameters*>(parameters);
    int64_t packet_duration = av_rescale_q(int64_t(1000 / video_parameers->frameRate())
                                , {1, 1000}, timeBase());
    _packet_dts_delta = packet_duration;
    _packet_pts_delta = packet_duration;
    _packet_duration = packet_duration;
    return YCode::OK;
}
