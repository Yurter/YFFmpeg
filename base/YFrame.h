#pragma once

#include "ffmpeg.h"
#include "YData.h"

class YFrame : public YData<AVFrame>
{

public:

    YFrame();
    YFrame(AVFrame* frame);
    ~YFrame();

    bool            alloc();

    bool            empty() const;
    std::string     toString() const;

private:

    // General

    // FFmpeg

};
