#include "YAudioStream.h"

YAudioStream::YAudioStream(YAudioParameters* param) :
    YAudioStream(nullptr, param)
{
    //
}

YAudioStream::YAudioStream(AVStream* stream, YAudioParameters* param ) :
    YStream(stream, YMediaType::MEDIA_TYPE_AUDIO, param)
{
    setName("YAudioStream");
}

YCode YAudioStream::init() //TODO
{
//    int64_t duration = av_rescale_q(int64_t(1000 / parameters.frameRate())
//                        , {1, 1000}, timeBase());
    int64_t duration = 23; //TODO
    _packet_dts_delta = duration;
    _packet_pts_delta = duration;
    _packet_duration = duration;
    setInited(true);
    return YCode::OK;
}
