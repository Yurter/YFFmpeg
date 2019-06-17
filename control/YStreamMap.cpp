#include "YStreamMap.h"

#include <algorithm>
#include <iterator>

YStreamMap::YStreamMap()
{
    setName("YStreamMap");
}

bool YStreamMap::addRoute(YStream* src_stream, YStream* dst_stream)
{
    if (src_stream == nullptr) { return false; }
    if (dst_stream == nullptr) { return false; }
    if (src_stream->raw() == dst_stream->raw())   { return false; }
    if (src_stream->type() != dst_stream->type()) { return false; }
    _map.push_back(YMap(src_stream, dst_stream));
    setInited(true);
    return true;
}

YCode YStreamMap::processInputData(YPacket& input_data) //TODO
{
    auto result = std::find_if(_map.begin(), _map.end(),
        [input_data](YMap& it) {
        return it.second->type() == input_data.type();
    });
    if (result == _map.end()) {
        input_data.setStreamIndex(INVALID_INT);
        return YCode::INVALID_INPUT;
    }
    input_data.setStreamIndex(result->second->index());
    sendOutputData(input_data);
    return YCode::OK;
}
