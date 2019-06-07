#include "YStream.h"

YStream::YStream() :
    YData<AVStream*>(nullptr, YMediaType::MEDIA_TYPE_UNKNOWN),
    _duration(0),
    _prev_pts(0),
    _prev_dts(0)
{
    //
}

YStream::YStream(AVStream *stream, YMediaType type) : //TODO вложить конструктор
    YData<AVStream*>(stream, type),
    _duration(0),
    _prev_pts(0),
    _prev_dts(0)
{
    //
}

YStream::~YStream()
{
    //
}

bool YStream::empty() const
{
    return true;
}

std::string YStream::toString() const
{
    return "TODO";
}

bool YStream::stampPacket(YPacket &packet)
{
    return false;
}

int64_t YStream::index() const
{
    return _data->index;
}

int64_t YStream::duration() const
{
    return _duration;
}

void YStream::increaseDuration(int64_t value)
{
    _duration += value;
}
