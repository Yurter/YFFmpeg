#include "YPacket.h"

YPacket::YPacket() :
    _type(YMediaType::MEDIA_TYPE_UNKNOWN)
{
    //
}

YPacket::~YPacket()
{
    // TODO ffmpeg ф-ии на отчистку
    //avpacket_unref(&m_raw);
}

AVPacket *YPacket::raw()
{
    return &_packet;
}

YMediaType YPacket::type() const
{
    return _type;
}

void YPacket::setType(YMediaType type)
{
    _type = type;
}

bool YPacket::isVideo() const
{
    return _type == YMediaType::MEDIA_TYPE_VIDEO;
}

bool YPacket::isAudio() const
{
    return _type == YMediaType::MEDIA_TYPE_AUDIO;
}

std::ostream& operator<<(std::ostream &os, const YPacket &pkt)
{
    /* Video packet: 33123 byte, dts 460, pts 460, duration 33 */
    auto dts_str = pkt._packet.dts == AV_NOPTS_VALUE ? "AV_NOPTS_VALUE" : std::to_string(pkt._packet.dts);
    auto pts_str = pkt._packet.pts == AV_NOPTS_VALUE ? "AV_NOPTS_VALUE" : std::to_string(pkt._packet.pts);
    os << utils::media_type_to_string(pkt._type) << " packet: "
       << pkt._packet.size << " bytes, "
       << "dts " << dts_str << ", "
       << "pts " << pts_str << ", "
       << "duration " << pkt._packet.duration;
    return os;
}
