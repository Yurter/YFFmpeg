#include "YStream.hpp"
#include "utils.hpp"

YStream::YStream(YParameters* param) :
    YStream(nullptr, param)
{
    EMPTY_CONSTRUCTOR
}

YStream::YStream(AVStream* stream, YParameters* param) :
    YData<AVStream*>(stream, param->type()),
    parameters(param),
    _context(nullptr),
    _uid(INVALID_INT),
    _duration(DEFAULT_INT),
    _used(false),
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
    /* Иницаиализация полей параметров кодека дефолтными значениями */
    try_to(utils::init_codecpar(codecParameters(), parameters->codec()));
    /* Инициализация полей параметров кодека значениями из параметров потока */
    utils::parameters_to_avcodecpar(parameters, codecParameters());
    return YCode::OK;
}

std::string YStream::toString() const
{
    return_if_not(inited(), "not inited!");
    std::string str = "[" + std::to_string(parameters->contextUid())
            + ":" + std::to_string(parameters->streamIndex()) + "] "
            + utils::media_type_to_string(type()) + " stream: "
            + parameters->codecName() + ", ";

    if (used()) {
        str += "dts_delta " + std::to_string(_packet_dts_delta) + ", "
                + "pts_delta " + std::to_string(_packet_pts_delta) + ", "
//              + "time_base " + utils::rational_to_string(timeBase()) + ", "
                + "time_base " + utils::rational_to_string(parameters->timeBase()) + ", "
                + "duration " + std::to_string(_duration);
    } else {
        str += "not used";
    }

    return str;
}

YCode YStream::stampPacket(YPacket& packet)
{
    return_if(packet.type() != type(), YCode::INVALID_INPUT);

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

void YStream::setContext(YObject* context)
{
    _context = context;
}

void YStream::setUid(int64_t uid)
{
    if (invalid_int(_uid)) { _uid = uid; }
}

void YStream::setUsed(bool used)
{
    _used = used;
}

YObject* YStream::context() const
{
    return _context;
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

bool YStream::used() const
{
    return _used;
}

AVCodecParameters* YStream::codecParameters()
{
    return _data->codecpar;
}

void YStream::increaseDuration(int64_t value)
{
    _duration += value;
}

bool YStream::operator>(const YStream& other) const
{
    return parameters->bitrate() > other.parameters->bitrate();
}

void YStream::parseParametres() //TODO ? што это?
{
    _data->time_base = parameters->timeBase();
}
