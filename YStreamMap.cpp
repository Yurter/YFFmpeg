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
    if (src_stream == nullptr) { return false; }
    if (dst_stream == nullptr) { return false; }
    if (src_stream->raw() == dst_stream->raw())   { return false; }
    if (src_stream->type() != dst_stream->type()) { return false; }
    _map.push_back(std::pair(src_stream, dst_stream));
    setInited(true);
    return true;
}

YCode YStreamMap::processInputData(YPacket &input_data)
{
    if (!inited()) { return YCode::NOT_INITED; }

//    auto result = std::find_if(_map.begin(), _map.end(),
//        [input_data](std::pair<YStream*,YStream*>& it) {
//        return it.first->index() == input_data.streamIndex();
//    });
    //TODO
    auto result = std::find_if(_map.begin(), _map.end(),
        [input_data](std::pair<YStream*,YStream*>& it) {
        return it.second->type() == input_data.type();
    });

    if (result == _map.end()) {
        input_data.setStreamIndex(INVALID_INT);
        return YCode::INVALID_INPUT;
    }

    std::cout << "map: " << utils::media_type_to_string(input_data.type()) << " -> " << result->second->index() << std::endl;

    input_data.setStreamIndex(result->second->index());
    sendOutputData(input_data);
    return YCode::OK;
}
