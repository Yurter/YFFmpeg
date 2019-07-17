#include "YAudioStream.hpp"
#include "../base/utils.hpp"

YAudioStream::YAudioStream(YAudioParameters* param) :
    YAudioStream(nullptr, param)
{
    EMPTY_CONSTRUCTOR
}

YAudioStream::YAudioStream(AVStream* stream, YAudioParameters* param ) :
    YStream(stream, param)
{
    setName("YAudioStream");
}

YCode YAudioStream::init() //TODO
{
    try_to(YStream::init());
//    int64_t duration = av_rescale_q(int64_t(1000 / parameters.frameRate())
//                        , {1, 1000}, timeBase());
    int64_t duration = 23; //TODO
    _packet_dts_delta = duration;
    _packet_pts_delta = duration;
    _packet_duration = duration;
    setInited(true);
    return YCode::OK;
}
