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

    friend std::ostream& operator<<(std::ostream& os, const YPacket& pkt);

private:

    //General
    YMediaType      _type;

    //FFmpeg
    AVPacket        _packet;

};
