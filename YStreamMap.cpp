#include "YStreamMap.h"

#include <algorithm>
#include <iterator>

YStreamMap::YStreamMap()
{
    //
}

YStreamMap::~YStreamMap()
{
    //
}

bool YStreamMap::map(YPacket &packet) //TODO
{
//    for (auto&& src_idx : _map) {
//        if ()
//    }
//    auto src_str_index = packet.raw().s
    auto result = std::find_if(_map.begin(), _map.end(), [](){});
    return true;
}
