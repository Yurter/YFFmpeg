#pragma once

#include "base/YAbstractCoder.h"
#include "base/YDataProcessor.h"
#include "../context/YDestination.h"

class YEncoder : public YAbstractCoder, public YDataProcessor<YFrame,YPacket>
{

public:

    YEncoder() = delete;
    YEncoder(YDestination *destination);
    virtual ~YEncoder() override;

    bool init() override;

private:

    bool initVideoCodec() override;
    bool initAudioCodec() override;
    YCode processInputData(YFrame& input_data) override;

private:

    // General
    YDestination*  _destination;

};
