#pragma once

#include "../base/YAbstractCodec.h"

class YEncoder : public YAbstractCodec<YFrame,YPacket>
{

public:

    YEncoder(YStream* stream);
    virtual ~YEncoder() override;

private:

    AVCodec*            findCodec() override;
    YCode               processInputData(YFrame& input_data) override;

};

