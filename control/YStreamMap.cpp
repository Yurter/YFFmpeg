#include "YStreamMap.h"

#include <algorithm>
#include <iterator>
#include <exception>

YStreamMap::YStreamMap() :
    _source_uid(DEFAULT_INT),
    _destination_uid(DEFAULT_INT)
{
    setName("YStreamMap");
}

stream_map& YStreamMap::map()
{
    return _stream_map;
}

YCode YStreamMap::addRoute(streams_pair streams)
{
    return_if_not(dynamic_cast<YSource*>(streams.first->mediaContext()), YCode::INVALID_INPUT);
    return_if_not(dynamic_cast<YDestination*>(streams.second->mediaContext()), YCode::INVALID_INPUT);
    streams.first->mediaContext()->setUid(_source_uid++);
    streams.second->mediaContext()->setUid(_destination_uid++);
    streams.first->setUid(utils::gen_stream_uid(streams.first->mediaContext()->uid(), streams.first->index()));
    streams.second->setUid(utils::gen_stream_uid(streams.second->mediaContext()->uid(), streams.second->index()));
    _stream_map.insert(streams);
}

YCode YStreamMap::setRoute(YStream* src_stream, YAsyncQueue<YPacket>* next_processor)
{
    return_if(invalid_int(src_stream->uid()), YCode::INVALID_INPUT);
    _route_map.insert({src_stream->uid(), next_processor});
    return YCode::OK;
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
