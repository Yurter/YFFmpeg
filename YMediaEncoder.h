#pragma once

#include "base/YAbstractCoder.h"
#include "YMediaDestination.h"

class YMediaEncoder : public YAbstractCoder
{

public:

    YMediaEncoder() = delete;
    YMediaEncoder(YMediaDestination *destination);
    virtual ~YMediaEncoder();

    bool init();

    bool encodeFrames(AVPacket *encoded_packet, std::list<AVFrame*> &decoded_frames);

private:

    bool initVideoCodec();
    bool initAudioCodec();

private:

    //General parametres
    YMediaDestination*  _destination;

};
