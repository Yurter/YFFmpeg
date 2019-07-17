#pragma once

#include "../base/YCodec.hpp"

class YEncoder : public YCodec<YFrame,YPacket>
{

public:

    YEncoder(YStream* stream);
    virtual ~YEncoder() override = default;

private:

    YCode               processInputData(YFrame& input_data) override;

};

