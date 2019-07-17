#include "YPacket.hpp"
#include "utils.hpp"

YPacket::YPacket() :
    YData<AVPacket>(),
    _stream_uid(INVALID_INT)
{
    setName("YPacket");
}

YPacket::~YPacket()
{
    // TODO ffmpeg ф-ии на отчистку
    //avpacket_unref(&m_raw);
}

YCode YPacket::init()
{
    av_init_packet(&_data);
    setInited(true);
    return YCode::OK;
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

void YPacket::setStreamUid(int64_t stream_uid)
{
    _stream_uid = stream_uid;
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

int64_t YPacket::streamUid() const
{
    return _stream_uid;
}

bool YPacket::empty() const
{
    return _data.size == 0;
}

std::string YPacket::toString() const
{
    /* Video packet: 33123 bytes, dts 460, pts 460, duration 33 */
    std::string str = utils::media_type_to_string(_type) + " packet: "
            + std::to_string(_data.size) + " bytes, "
            + "dts " + utils::pts_to_string(_data.dts) + ", "
            + "pts " + utils::pts_to_string(_data.pts) + ", "
            + "duration " + std::to_string(_data.duration) + ", "
            + "stream index " + std::to_string(_data.stream_index) + ", "
            + "stream uid " + std::to_string(_stream_uid);
    return str;
}
