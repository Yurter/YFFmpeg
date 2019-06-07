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

bool YStreamMap::addRoute(YStream* src_stream, YStream* dst_stream)
{
    if (src_stream->raw() == dst_stream->raw())   { return false; }
    if (src_stream->type() != dst_stream->type()) { return false; }
    _map.push_back({src_stream, dst_stream});
    setInited(true);
    return true;
}

YCode YStreamMap::processInputData(YPacket &input_data)
{
    if (!inited()) { return YCode::NOT_INITED; }

    auto result = std::find_if(_map.begin(), _map.end(),
        [input_data](std::pair<YStream*,YStream*>& it) {
        return it.first->index() == input_data.streamIndex();
    });

    if (result == _map.end()) {
        input_data.setStreamIndex(INVALID_INT);
        return YCode::INVALID_INPUT;
    }

    input_data.setStreamIndex(result->second->index());
    return YCode::OK;
}
