#pragma once

extern "C" {
#include <libavformat/avformat.h>
}

#include <list>

#include "YMediaDestination.h"
#include "YAbstractCoder.h"

class YMediaEncoder : public YAbstractCoder
{
public:

    YMediaEncoder() = delete;
    YMediaEncoder(YMediaDestination *destination);
    ~YMediaEncoder();

    bool init();

    AVPacket encodeFrames(std::list<AVFrame> &&frames);

private:

    bool initVideoCodec();
    bool initAudioCodec();

private:

    YMediaDestination       *_destination;

};
