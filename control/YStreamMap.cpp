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
    try_to(checkStreamPair(streams));
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
    setInited(true);
    return YCode::OK;
}

YCode YStreamMap::checkStreamPair(streams_pair streams)
{
    return_if(streams.first == nullptr, YCode::INVALID_INPUT);
    return_if(streams.second == nullptr, YCode::INVALID_INPUT);
    return_if(streams.first->raw() == streams.second->raw(), YCode::INVALID_INPUT);
    return_if_not(streams.first->type() == streams.second->type(), YCode::INVALID_INPUT);
    return_if_not(dynamic_cast<YSource*>(streams.first->mediaContext()), YCode::INVALID_INPUT);
    return_if_not(dynamic_cast<YDestination*>(streams.second->mediaContext()), YCode::INVALID_INPUT);
    return YCode::OK;
}

YCode YStreamMap::processInputData(YPacket& input_data)
{
    YStream* input_stream = input_data.stream();
    YStream* output_stream = nullptr;

    try {
        output_stream = _stream_map.at(input_stream);
    } catch (std::out_of_range) { return YCode::INVALID_INPUT; }

    input_data.setStreamIndex(output_stream->index());

    YNextProcessor* next_proc = nullptr;
    try {
        next_proc = _route_map.at(input_stream->uid());
    } catch (std::out_of_range) { return YCode::INVALID_INPUT; }

    return sendOutputData(input_data, next_proc);
}

