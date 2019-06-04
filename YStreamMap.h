#pragma once

#include "base/YPacket.h"
#include "YMediaSource.h"
#include "YMediaDestination.h"
#include <map>

class YStreamMap
{

public:

    YStreamMap();
    ~YStreamMap();

    bool init(YMediaSource* source, YMediaDestination* destination);
    bool map(YPacket& packet);

private:

    // General
    std::map<int64_t, int64_t>      _map;

};
