#include "YStream.h"

YStream::YStream() :
    YStream(nullptr, nullptr, YMediaType::MEDIA_TYPE_UNKNOWN)
{
    //
}

YStream::YStream(YAbstractMedia* media_context, AVStream* stream, YMediaType type) :
    YData<AVStream*>(stream, type),
    _media_context(media_context),
    _duration(DEFAULT_INT),
    _prev_dts(DEFAULT_INT),
    _prev_pts(DEFAULT_INT),
    _packet_index(DEFAULT_INT),
    _packet_dts_delta(INVALID_INT),
    _packet_pts_delta(INVALID_INT),
    _packet_duration(INVALID_INT)
{
    setName("YStream");
}

bool YStream::inited() const
{
    if (not_inited_int(_packet_dts_delta))  { return false; }
    if (not_inited_int(_packet_pts_delta))  { return false; }
    if (not_inited_int(_packet_duration))   { return false; }
    return true;
}

std::string YStream::toString() const
{
    std::string str = utils::media_type_to_string(_type) + " stream: "
            + "dts_delta " + std::to_string(_packet_dts_delta) + ", "
            + "pts_delta " + std::to_string(_packet_pts_delta) + ", "
            + "time_base " + utils::rational_to_string(timeBase()) + ", "
            + "duration " + std::to_string(_duration);
    return str;
}

bool YStream::stampPacket(YPacket& packet)
{
    if (packet.type() != type()) {
        log_error(utils::code_to_string(YCode::INVALID_INPUT));
        return false;
    }

    packet.setDts(_prev_dts);
    packet.setPts(_prev_pts);
    packet.setDuration(_packet_duration);
    packet.setPos(-1);

    _prev_dts += _packet_dts_delta;
    _prev_pts += _packet_pts_delta;
    _duration += _packet_duration;
    _packet_index++;

    return true;
}

int64_t YStream::index() const
{
    return _data->index;
}

int64_t YStream::duration() const
{
    return _duration;
}

AVRational YStream::timeBase() const
{
    return _data->time_base;
}

AVCodecParameters* YStream::codecParameters()
{
    return _data->codecpar;
}

YAbstractMedia* YStream::mediaContext()
{
    return _media_context;
}

void YStream::increaseDuration(int64_t value)
{
    _duration += value;
}
