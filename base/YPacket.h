#pragma once

#include "YData.h"
#include "utils.h"
#include <iostream>

class YPacket : public YData<AVPacket>
{

public:

    YPacket();
    ~YPacket();

    void            init();

    void            setPts(int64_t pts);
    void            setDts(int64_t dts);
    void            setDuration(int64_t duration);
    void            setPos(int64_t pos);
    void            setStreamIndex(int64_t stream_index);

    int64_t         pts()           const;
    int64_t         dts()           const;
    int64_t         duration()      const;
    int64_t         pos()           const;
    int64_t         streamIndex()   const;

    bool            empty()         const;
    std::string     toString()      const;

private:

    // General

    // FFmpeg

};
