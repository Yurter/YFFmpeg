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

YCode YStreamMap::addRoute(media_stream source, media_stream destination)
{
    source.first->setUid(_source_uid++);
    destination.first->setUid(_destination_uid++);

    _stream_map.insert(source);
    _index_map.insert({source.first->uid(), source.second->index()});
    return YCode::OK;
}

//YCode YStreamMap::addRoute(media_stream source, media_stream destination)
//{
//    try_to(checkStreamPair(source));
//    source.first->mediaContext()->setUid(_source_uid++);
//    source.second->mediaContext()->setUid(_destination_uid++);
//    source.first->setUid(utils::gen_stream_uid(source.first->mediaContext()->uid(), source.first->index()));
//    source.second->setUid(utils::gen_stream_uid(source.second->mediaContext()->uid(), source.second->index()));
//    _stream_map.insert(source);
//    _index_map.insert({source.first->uid(), source.second->index()});
//    return YCode::OK;
//}

YCode YStreamMap::setRoute(YStream* src_stream, YAsyncQueue<YPacket>* next_processor)
{
    return_if(invalid_int(src_stream->uid()), YCode::INVALID_INPUT);
    _packet_map.insert({src_stream->uid(), next_processor});
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
    /* Определение локального индекса выходного потока */
    int64_t out_stream_index = INVALID_INT;
    try {
        out_stream_index = _index_map.at(input_data.streamUid());
    } catch (std::out_of_range) { return YCode::INVALID_INPUT; }

    /* Определение первого обработчика пакета */
    YNextProcessor* next_proc = nullptr;
    try {
        next_proc = _packet_map.at(input_data.streamUid());
    } catch (std::out_of_range) { return YCode::INVALID_INPUT; }

    input_data.setStreamIndex(out_stream_index);
    return sendOutputData(input_data, next_proc);
}
