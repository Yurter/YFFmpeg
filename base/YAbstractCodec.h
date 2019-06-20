#pragma once

#include "ffmpeg.h"
#include "YThread.h"
#include "../base/YStream.h"
#include "../base/YFrame.h"
#include "../base/YPacket.h"
#include <string>

class YAbstractCodec : virtual public YThread
{

public:

    YAbstractCodec(YStream* stream);
    virtual ~YAbstractCodec();

    virtual YCode       init() final;

    AVCodecContext*     codecContext();

private:

    YCode               copyCodecParameters(YStream* stream);

protected:

    // General
    YMediaType          _type;

    // FFmpeg
    AVCodecContext*     _codec_context;

};
