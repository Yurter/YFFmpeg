#pragma once

#include "base/YPacket.h"
#include "base/YDataProcessor.h"
#include "../context/YSource.h"
#include "../context/YDestination.h"
#include <map>

typedef std::map<YStream*,YAsyncQueue<YPacket>> YMap;

class YStreamMap : public YDataProcessor<YPacket,YPacket>
{

public:

    YStreamMap();
    virtual ~YStreamMap() override = default;

    bool        addRoute(YStream* src_stream, YAsyncQueue<YPacket>* next_processor);
    YCode       processInputData(YPacket& input_data) override;

private:

    // General
    YMap        _route_map;

};
