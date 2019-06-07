#pragma once

#include "ffmpeg.h"
#include "YPacket.h"

class YStream : public YData<AVStream*>
{

public:

    YStream();
    YStream(AVStream *stream, YMediaType type);
    ~YStream();

    bool                empty()     const;
    std::string         toString()  const;

    bool                stampPacket(YPacket& packet);

    int64_t             index()     const;
    int64_t             duration()  const;
    void                increaseDuration(int64_t value);

private:

    // General
    int64_t             _duration;
    int64_t             _prev_pts;
    int64_t             _prev_dts;

};
