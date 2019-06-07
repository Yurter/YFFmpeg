#pragma once

#include "base/YPacket.h"
#include "base/YDataProcessor.h"
#include "YSource.h"
#include "YDestination.h"
#include <map>

class YStreamMap : public YDataProcessor<YPacket,YPacket>
{

public:

    YStreamMap();
    virtual ~YStreamMap();

    bool addMap(YStream src_stream, YStream dst_stream);
    YCode processInputData(YPacket& input_data);

private:

    // General
    std::map<YStream, YStream>      _map;

};
