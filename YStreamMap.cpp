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

bool YStreamMap::map(YStream src_stream, YStream dst_stream)
{
    if (src_stream.raw() == dst_stream.raw())   { return false; }
    if (src_stream.type() != dst_stream.type()) { return false; }
//    _map.insert(src_stream, dst_stream);
    return true;
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
