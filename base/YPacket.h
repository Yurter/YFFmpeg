#pragma once

#include "YData.h"
#include "utils.h"
#include <iostream>

class YPacket : YData<AVPacket>
{

public:

    YPacket();
    ~YPacket();

    bool            empty() const;
    std::string     toString() const;

private:

    // General

    // FFmpeg

};
