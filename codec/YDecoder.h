#pragma once

#include "../base/YAbstractCodec.h"

class YDecoder : public YAbstractCodec<YPacket,YFrame>
{

public:

    YDecoder(YStream* stream);
    virtual ~YDecoder() override = default;

private:

    AVCodec*            findCodec() override;
    YCode               processInputData(YPacket& input_data) override;

};
