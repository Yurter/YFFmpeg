#include "YPacket.h"

YPacket::YPacket() :
    YData<AVPacket>()
{
    setName("YPacket");
}

YPacket::~YPacket()
{
    // TODO ffmpeg ф-ии на отчистку
    //avpacket_unref(&m_raw);
}

bool YPacket::init()
{
    av_init_packet(&_data);
    return true;
}

void YPacket::setPts(int64_t pts)
{
    _data.pts = pts;
}

void YPacket::setDts(int64_t dts)
{
    _data.dts = dts;
}

void YPacket::setDuration(int64_t duration)
{
    _data.duration = duration;
}

void YPacket::setPos(int64_t pos)
{
    _data.pos = pos;
}

void YPacket::setStreamIndex(int64_t stream_index)
{
    _data.stream_index = static_cast<int>(stream_index);
}

int64_t YPacket::pts() const
{
    return _data.pts;
}

int64_t YPacket::dts() const
{
    return _data.dts;
}

int64_t YPacket::duration() const
{
    return _data.duration;
}

int64_t YPacket::pos() const
{
    return _data.pos;
}

int64_t YPacket::streamIndex() const
{
    return static_cast<int64_t>(_data.stream_index);
}

bool YPacket::empty() const
{
    return _data.size == 0;
}

std::string YPacket::toString() const
{
    /* Video packet: 33123 byte, dts 460, pts 460, duration 33 */
    auto dts_str = _data.dts == AV_NOPTS_VALUE ? "NOPTS" : std::to_string(_data.dts);
    auto pts_str = _data.pts == AV_NOPTS_VALUE ? "NOPTS" : std::to_string(_data.pts);
    std::string str = utils::media_type_to_string(_type) + " packet: "
            + std::to_string(_data.size) + " bytes, "
            + "dts " + dts_str + ", "
            + "pts " + pts_str + ", "
            + "duration " + std::to_string(_data.duration);
    return str;
}
