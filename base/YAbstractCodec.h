#pragma once

#include "YThread.h"
#include "../base/YStream.h"
#include "../base/YFrame.h"
#include "../base/YPacket.h"

class YAbstractCodec : virtual public YThread
{

public:

    YAbstractCodec(YStream* stream);
    virtual ~YAbstractCodec();

    virtual YCode       init() final;

    AVCodecContext*     codecContext();

protected:

    virtual AVCodec*    findCodec() = 0;

protected:

    // Media
    YStream*            _stream;

    // FFmpeg
    AVCodecContext*     _codec_context;

};
