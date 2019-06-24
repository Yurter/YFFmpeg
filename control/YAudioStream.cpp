#include "YAudioStream.h"

YAudioStream::YAudioStream() :
    YAudioStream(nullptr, nullptr)
{
    //
}

YAudioStream::YAudioStream(YAbstractMedia* media_context, AVStream* stream) :
    YStream(media_context, stream, YMediaType::MEDIA_TYPE_AUDIO)
{
    setName("YAudioStream");
}

bool YAudioStream::init() //TODO
{
//    int64_t duration = av_rescale_q(int64_t(1000 / parameters.frameRate())
//                        , {1, 1000}, timeBase());
    int64_t duration = 23; //TODO
    _packet_dts_delta = duration;
    _packet_pts_delta = duration;
    _packet_duration = duration;
    return true;
}
