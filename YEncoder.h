#pragma once

#include "base/YAbstractCoder.h"
#include "base/YDataProcessor.h"
#include "YDestination.h"

class YMediaEncoder : public YAbstractCoder, public YDataProcessor<YFrame,YPacket>
{

public:

    YMediaEncoder() = delete;
    YMediaEncoder(YMediaDestination *destination);
    virtual ~YMediaEncoder() override;

    bool init() override;

private:

    bool initVideoCodec() override;
    bool initAudioCodec() override;
    YCode run() override;

private:

    //General parametres
    YMediaDestination*  _destination;

};
