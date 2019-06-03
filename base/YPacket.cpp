#include "YPacket.h"

YPacket::YPacket() :
    YData<AVPacket>()
{
    //
}

YPacket::~YPacket()
{
    // TODO ffmpeg ф-ии на отчистку
    //avpacket_unref(&m_raw);
}

void YPacket::init()
{
    av_init_packet(raw());
}

bool YPacket::empty() const
{
    return _data.size == 0;
}

std::string YPacket::toString() const
{
    /* Video packet: 33123 byte, dts 460, pts 460, duration 33 */
    auto dts_str = _data.dts == AV_NOPTS_VALUE ? "AV_NOPTS_VALUE" : std::to_string(_data.dts);
    auto pts_str = _data.pts == AV_NOPTS_VALUE ? "AV_NOPTS_VALUE" : std::to_string(_data.pts);
    // + utils::media_type_to_string(pkt._type) << " packet: " //TODO
    std::string str =
       std::to_string(_data.size) + " bytes, "
            + "dts " + dts_str + ", "
            + "pts " + pts_str + ", "
            + "duration " + std::to_string(_data.duration);
    return str;
}
