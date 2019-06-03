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

    bool            empty() const;
    std::string     toString() const;

private:

    // General

    // FFmpeg

};
