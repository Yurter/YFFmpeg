#include "YStreamMap.h"

#include <algorithm>
#include <iterator>
#include <exception>

YStreamMap::YStreamMap() :
    _source_uid(DEFAULT_INT)
{
    setName("YStreamMap");
}

YCode YStreamMap::addRoute(YStream *src_stream, YStream *dst_stream)
{
    src_stream->mediaContext()->setUid(_source_uid++);
    auto packet_index = utils::gen_packet_index(src_stream->mediaContext()->uid(), src_stream->index());
    _route_map.insert({packet_index,});
}

bool YStreamMap::addRoute(YStream *src_stream, YAsyncQueue<YPacket> *next_processor)
{
    //
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
    try {
//        auto&& route = _route_map.at("")
    } catch (std::out_of_range) {
        log_error(utils::code_to_string(YCode::INVALID_INPUT));
    }
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
