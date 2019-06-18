#pragma once

#include "ffmpeg.h"
//#include "YPacket.h"
//#include "YFrame.h"
#include "YThread.h"
#include "../base/YStream.h"
#include <string>

class YAbstractCodec : virtual public YThread
{

public:

    YAbstractCodec();
    virtual ~YAbstractCodec();

    virtual YCode       init(YStream* stream) final;

    AVCodecContext*     codecContext();

private:

    YCode               copyCodecParameters(YStream* stream);

protected:

    // General
    YMediaType          _type;

    // FFmpeg
    AVCodecContext*     _codec_context;

};
