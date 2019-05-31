#pragma once

#include "ffmpeg.h"
#include "utils.h"
#include <iostream>

class YPacket
{

public:

    YPacket();
    ~YPacket();

    AVPacket*       raw();

    void            setType(YMediaType type);

    bool            isVideo() const;
    bool            isAudio() const;

    friend std::ostream& operator<<(std::ostream& os, const YPacket& pkt);

private:

    // General
    YMediaType      _type;

    // FFmpeg
    AVPacket        _packet;

};
