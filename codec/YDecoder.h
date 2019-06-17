#pragma once

#include "../base/YAbstractCodec.h"
#include "../base/YDataProcessor.h"

class YDecoder : public YAbstractCodec, public YDataProcessor<YPacket,YFrame>
{

public:

    YDecoder();
    virtual ~YDecoder() override;

private:

    YCode processInputData(YPacket& input_data) override;

private:

    // General

};
