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

bool YStreamMap::init(YMediaSource* source, YMediaDestination* destination)
{
//    // упрощенная схема - [0,1] видео- и аудио потоки у источника и деста
//    for (auto src_stream : *source->streams()) {
//        _map.insert({});
//    }
//    auto
//    source->stream()
    return false;
}

bool YStreamMap::map(YPacket& packet) //TODO
{
//    for (auto&& src_idx : _map) {
//        if ()
//    }
//    auto src_str_index = packet.raw().s
//    auto result = std::find_if(_map.begin(), _map.end(), [](){});
    return true;
}
