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
    YMediaEncoder(const YMediaDestination *destination);

    bool init();

    AVPacket encodeFrames(std::list<AVFrame> &&frames);

	~YMediaEncoder();

private:

    bool initVideoCodec();
    bool initAudioCodec();

};
