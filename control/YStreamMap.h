#pragma once

#include "base/YPacket.h"
#include "base/YDataProcessor.h"
#include "../context/YSource.h"
#include "../context/YDestination.h"
#include <vector>

typedef std::pair<YStream*,YStream*> YMap;

class YStreamMap : public YDataProcessor<YPacket,YPacket>
{

public:

    YStreamMap();
    virtual ~YStreamMap() override = default;

    bool        addRoute(YStream* src_stream, YStream* dst_stream);
    YCode       processInputData(YPacket& input_data) override;

private:

    // General
    std::vector<YMap>   _map; //TODO change vector to map

};
