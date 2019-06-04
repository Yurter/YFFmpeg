#pragma once

#include "base/YPacket.h"
#include <map>

class YStreamMap
{

public:

    YStreamMap();
    ~YStreamMap();

    bool map(YPacket& packet);

private:

    // General
    std::map<int64_t, int64_t>      _map;

};
