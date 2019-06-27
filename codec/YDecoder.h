#pragma once

#include "../base/YAbstractCodec.h"

class YDecoder : public YAbstractCodec<YPacket,YFrame>
{

public:

    YDecoder(YStream* stream);
    virtual ~YDecoder() override;

private:

    AVCodec*            findCodec() override;
    YCode               processInputData(YPacket& input_data) override;

};
