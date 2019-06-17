#pragma once

#include "base/YAbstractCodec.h"
#include "base/YDataProcessor.h"
#include "../control/YVideoStream.h"

class YVideoDecoder : public YAbstractCodec, public YDataProcessor<YPacket,YFrame>
{

public:

    YVideoDecoder() = default;
    virtual ~YVideoDecoder() override;

    bool        init(YStream *stream) override;

private:

    virtual YCode processInputData(YPacket& input_data) override;

private:

    // General

};
