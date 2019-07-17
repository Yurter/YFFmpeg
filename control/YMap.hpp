#pragma once

#include "base/YPacket.hpp"
#include "base/YDataProcessor.hpp"
#include "../context/YSource.hpp"
#include "../context/YDestination.hpp"
#include <map>

/* Таблица соответствий входного и выходного потоков */
typedef std::map<stream_context,stream_context> stream_map;

/* Таблица соответствий uid входного потока и указателя на первый в последовательности обработчик пакета */
typedef std::map<int64_t,YAsyncQueue<YPacket>*> packet_map;

/* Таблица соответствий uid входного потока и локального индекса выходного потока */
typedef std::map<int64_t,int64_t>               index_map;

/* ? */
typedef std::pair<stream_context, stream_context> Route;
typedef std::list<YObject*> ProcessorList;

class YMap : public YDataProcessor<YPacket,YPacket>
{

public:

    YMap();
    virtual ~YMap() override = default;

    YCode               init() override;
    std::string         toString() const override; //TODO

    stream_map&         streamMap();
    packet_map&         packetMap();
    index_map&          indexMap();

    ProcessorList&      getExtraProcessors();

    YCode               addRoute(YStream* in_stream, YStream* out_stream); //TODO
    YCode               addRoute(Route route);
    YCode               setRoute(YStream* src_stream, YAsyncQueue<YPacket>* next_processor);

private:

    YCode               processInputData(YPacket& input_data) override;

private:

    stream_map          _stream_map;
    packet_map          _packet_map;
    index_map           _index_map;

    ProcessorList       _extra_processors;

};
