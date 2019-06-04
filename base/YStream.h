#pragma once

#include "ffmpeg.h"
#include "YData.h"

class YStream : public YData<AVStream*>
{

public:

    YStream();
    YStream(AVStream *stream, YMediaType type);
    ~YStream();

    bool empty() const;
    std::string toString() const;

    int64_t             index() const;

private:

    // General

};
