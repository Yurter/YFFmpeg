#include "YMap.hpp"

#include <algorithm>
#include <iterator>
#include <exception>

YMap::YMap()
{
    setName("YMap");
}

YCode YMap::init()
{
    //TODO
}

std::string YMap::toString() const
{
    return "TODO"; //вывод цепочек процессоров
}

StreamMap* YMap::streamMap()
{
    return &_stream_map;
}

YCode YMap::addRoute(YStream* in_stream, YStream* out_stream)
{
    return_if(in_stream->type() != out_stream->type(), YCode::INVALID_INPUT);
    return_if(in_stream->uid() == out_stream->uid(),   YCode::INVALID_INPUT);
    _stream_map.insert({ in_stream, out_stream });
    return YCode::OK;
}

YCode YMap::setRoute(YStream* src_stream, PacketProcessor* next_processor)
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
    NextProcessor* next_proc = nullptr;
    try {
        next_proc = _packet_map.at(input_data.streamUid());
    } catch (std::out_of_range) { return YCode::INVALID_INPUT; }

    /* Инициализаця индекса потока и отправка пакета */
    input_data.setStreamIndex(out_stream_index);
    return sendOutputData(input_data, next_proc);
}
