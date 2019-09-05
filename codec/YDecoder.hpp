#pragma once
#include "../base/YCodec.hpp"

class YDecoder : public YCodec<YPacket,YFrame> {

public:

    YDecoder(YStream* stream);
    virtual ~YDecoder() override = default;

private:

    YCode processInputData(YPacket& input_data) override;

};
