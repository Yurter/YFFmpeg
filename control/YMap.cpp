#include "YMap.hpp"

#include <algorithm>
#include <iterator>
#include <exception>

YMap::YMap()
{
    setName("YMap");
}

YCode YMap::init() //TODO
{
    return YCode::ERR;
}

std::string YMap::toString() const
{
    return "TODO";
}

stream_map& YMap::streamMap()
{
    return _stream_map;
}

packet_map& YMap::packetMap()
{
    return _packet_map;
}

index_map& YMap::indexMap()
{
    return _index_map;
}

ProcessorList& YMap::getExtraProcessors()
{
    return _extra_processors;
}

YCode YMap::addRoute(Route route)
{
    /* Соответствие входного и выходного потоков */
    _stream_map.insert({route.first, route.second});
    /* Соответствие uid входного потока и локального id выходного потока */
    auto source_context_uid = route.first.first->uid();
    auto sourcestram_uid = route.first.second;
    auto destination_stream_id = route.second.second;
    _index_map.insert({utils::gen_stream_uid(source_context_uid, sourcestram_uid), destination_stream_id});
    return YCode::OK;
}

YCode YMap::setRoute(YStream* src_stream, YAsyncQueue<YPacket>* next_processor)
{
    return_if(invalid_int(src_stream->uid()), YCode::INVALID_INPUT);
    _packet_map.insert({src_stream->uid(), next_processor});
    setInited(true);
    return YCode::OK;
}

YCode YMap::processInputData(YPacket& input_data)
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
