#pragma once

#include "base/YPacket.h"
#include "YSource.h"
#include "YDestination.h"
#include <map>

class YStreamMap
{

public:

    YStreamMap();
    ~YStreamMap();

    bool init(YSource* source, YMediaDestination* destination);
    bool map(YStream src_stream, YStream dst_stream);
    bool map(YPacket& packet);

private:

    // General
//    std::map<int64_t, int64_t>      _map;
    std::map<YStream, YStream>      _map;

};
