#include "YStream.h"
#include "utils.h"

YStream::YStream(YParameters* param) :
    YStream(nullptr, YMediaType::MEDIA_TYPE_UNKNOWN, param)
{
    //
}

YStream::YStream(AVStream* stream, YMediaType type, YParameters* param) :
    YData<AVStream*>(stream, type),
    parameters(param),
    _uid(INVALID_INT),
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

YStream::~YStream()
{
    delete parameters;
}

YCode YStream::init()
{
    return YCode::ERR;
}

std::string YStream::toString() const
{
    if_not(inited()) { return "not inited!"; }
    std::string str = "[" + std::to_string(parameters->contextUid())
            + ":" + std::to_string(parameters->streamIndex()) + "] "
            + utils::media_type_to_string(_type) + " stream: "
            + parameters->codecName() + ", "
            + "dts_delta " + std::to_string(_packet_dts_delta) + ", "
            + "pts_delta " + std::to_string(_packet_pts_delta) + ", "
            + "time_base " + utils::rational_to_string(timeBase()) + ", "
            + "duration " + std::to_string(_duration);
    return str;
}

YCode YStream::stampPacket(YPacket& packet)
{
    return_if_not(inited(), YCode::NOT_INITED);//TODO
    if (packet.type() != type()) {
        log_error(utils::code_to_string(YCode::INVALID_INPUT));
        return YCode::INVALID_INPUT;
    }

    packet.setDts(_prev_dts);
    packet.setPts(_prev_pts);
    packet.setDuration(_packet_duration);
    packet.setPos(-1);

    _prev_dts += _packet_dts_delta;
    _prev_pts += _packet_pts_delta;
    _duration += _packet_duration;
    _packet_index++;

    return YCode::OK;
}

void YStream::setUid(int64_t uid)
{
    if (invalid_int(_uid)) { _uid = uid; }
}

void YStream::setTimeBase(AVRational time_base)
{
    _data->time_base = time_base;
}

int64_t YStream::uid() const
{
    return _uid;
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

void YStream::increaseDuration(int64_t value)
{
    _duration += value;
}

//YParameters* YStream::parameters()
//{
//    return _parameters;
//}
