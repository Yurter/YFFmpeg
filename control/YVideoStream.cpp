#include "YVideoStream.h"

YVideoStream::YVideoStream() :
    YVideoStream(nullptr, nullptr)
{
    //
}

YVideoStream::YVideoStream(YAbstractMedia* media_context, AVStream* stream) :
    YStream(media_context, stream, YMediaType::MEDIA_TYPE_VIDEO)
{
    setName("YVideoStream");
}

bool YVideoStream::init()
{
    int64_t duration = av_rescale_q(int64_t(1000 / parameters.frameRate())
                        , {1, 1000}, timeBase());
    _packet_dts_delta = duration;
    _packet_pts_delta = duration;
    _packet_duration = duration;
    return true;
}
