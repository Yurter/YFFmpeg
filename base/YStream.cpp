#include "YStream.h"

YStream::YStream() :
    YData<AVStream*>(),
    _duration(0)
{
    //
}

YStream::YStream(AVStream *stream, YMediaType type) :
    YData<AVStream*>(stream, type),
    _duration(0)
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
