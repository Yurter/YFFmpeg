#pragma once

#include "../base/YAbstractCodec.h"
#include "../base/YDataProcessor.h"

class YEncoder : public YAbstractCodec, public YDataProcessor<YFrame,YPacket>
{

public:

    YEncoder(YStream* stream);
    virtual ~YEncoder() override;

private:

    YCode processInputData(YFrame& input_data) override;

};

