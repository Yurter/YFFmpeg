#include "YStreamMap.h"

#include <algorithm>
#include <iterator>
#include <exception>

YStreamMap::YStreamMap()
{
    setName("YStreamMap");
}

stream_map& YStreamMap::map()
{
    return _stream_map;
}

YCode YStreamMap::addRoute(stream_context source, stream_context destination)
{
    /* ? */
    _stream_map.insert({source, destination});
    /* Соответствие uid входного потока и локального id выходного потока */
    _index_map.insert(source.first->uid(), destination.second);
    return YCode::OK;
}

YCode YStreamMap::setRoute(YStream* src_stream, YAsyncQueue<YPacket>* next_processor)
{
    return_if(invalid_int(src_stream->uid()), YCode::INVALID_INPUT);
    _packet_map.insert({src_stream->uid(), next_processor});
    setInited(true);
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
