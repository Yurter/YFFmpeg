#include "YStream.h"

YStream::YStream() :
    YData<AVStream*>()
{
    //
}

YStream::YStream(AVStream *stream, YMediaType type) :
    YData<AVStream*>(stream, type)
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
