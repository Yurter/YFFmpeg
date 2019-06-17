#pragma once

#include "../base/YAbstractCodec.h"
#include "../base/YDataProcessor.h"

class YEncoder : YAbstractCodec, public YDataProcessor<YFrame,YPacket>
{

public:

    YEncoder();
    virtual ~YEncoder() override;

private:

    YCode processInputData(YFrame& input_data) override;

private:

    // General

};

