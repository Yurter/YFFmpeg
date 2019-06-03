#pragma once

#include "base/YAbstractCoder.h"
#include "YMediaDestination.h"

class YMediaEncoder : public YAbstractCoder
{

public:

    YMediaEncoder() = delete;
    YMediaEncoder(YMediaDestination *destination);
    virtual ~YMediaEncoder() override;

    bool init() override;

    //bool encodeFrames(YPacket& encoded_packet, std::list<AVFrame*>& decoded_frames);

private:

    bool initVideoCodec() override;
    bool initAudioCodec() override;
    YCode run() override;

private:

    //General parametres
    YMediaDestination*  _destination;

};
