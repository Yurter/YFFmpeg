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

    bool encodeFrames(AVPacket *encoded_packet, std::list<AVFrame*> &decoded_frames);

private:

    bool initVideoCodec();
    bool initAudioCodec();

private:

    YMediaDestination       *_destination;

};
