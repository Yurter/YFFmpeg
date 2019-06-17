#pragma once

#include "ffmpeg.h"
#include "YData.h"

class YFrame : public YData<AVFrame*>
{

public:

    YFrame();
    YFrame(AVFrame* frame);
    virtual ~YFrame() override;

    bool            alloc();
    void            free();

    bool            empty()     const;
    bool            inited()    const override;
    std::string     toString()  const override;

private:

    // General

};
