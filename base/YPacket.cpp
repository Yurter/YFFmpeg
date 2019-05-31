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
    os << utils::mediaTypeToString(pkt._type) << " packet: "
       << pkt._packet.size << " bytes, "
       << "dts " << pkt._packet.dts << ","
       << "pts " << pkt._packet.pts << ","
       << "duration " << pkt._packet.duration;
    return os;
}
